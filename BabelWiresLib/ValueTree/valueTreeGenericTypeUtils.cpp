/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

const babelwires::ValueTreeNode* babelwires::tryGetGenericTypeFromVariable(const ValueTreeNode& valueTreeNode) {
    auto variableData = TypeVariableData::isTypeVariable(valueTreeNode.getTypeRef());
    assert(variableData && "ValueTreeNode is not a type variable");

    unsigned int level = variableData->m_numGenericTypeLevels;
    const ValueTreeNode* current = &valueTreeNode;
    const ValueTreeNode* parent;
    do {
        parent = current->getOwner();
        if (!parent) {
            // This could happen if a subtree beneath a generic type was dragged out of a node,
            // TODO: This is not a useful state, so do something to prevent it.
            return nullptr;
        } else {
            if (parent->getType().as<GenericType>()) {
                if (level == 0) {
                    current = parent;
                    break;
                } else {
                    --level;
                }
            }
        }
        current = parent;
    } while (true);
    if (variableData->m_typeVariableIndex >= current->getType().is<GenericType>().getNumVariables()) {
        return nullptr;
    }

    return current;
}

namespace {
    int getMaximumPossibleHeightOfUnassignedGenericType(const babelwires::ValueTreeNode& valueTreeNode) {
        int height = -1;
        // If the top of the tree has generic types with all variables assigned, then they don't count towards the
        // height. This variable is used to count exclude those.
        int consecutiveAssigned = 0;
        const babelwires::ValueTreeNode* current = &valueTreeNode;
        const babelwires::ValueTreeNode* parent = current->getOwner();
        while (parent) {
            if (const babelwires::GenericType* genericType = parent->getType().as<babelwires::GenericType>()) {
                ++height;
                if (genericType->isAnyTypeVariableUnassigned(parent->getValue())) {
                    consecutiveAssigned = 0;
                } else {
                    ++consecutiveAssigned;
                }
            }
            current = parent;
            parent = current->getOwner();
        }
        return height - consecutiveAssigned;
    }

    bool typeRefContainsTypeVariable(const babelwires::TypeRef& typeRef) {
        struct Visitor {
            bool operator()(const std::monostate&) { return false; }
            bool operator()(babelwires::RegisteredTypeId) {
                // Registered types are assumed to be completely resolved.
                return false;
            }
            bool operator()(babelwires::TypeConstructorId constructorId,
                            const babelwires::TypeConstructorArguments& arguments) {
                unsigned int genericTypeDepth = m_genericTypeDepth;
                if (constructorId == babelwires::GenericTypeConstructor::getThisIdentifier()) {
                    ++genericTypeDepth;
                } else if (constructorId == babelwires::TypeVariableTypeConstructor::getThisIdentifier()) {
                    const auto typeVarData =
                        babelwires::TypeVariableTypeConstructor::extractValueArguments(arguments.getValueArguments());
                    if ((typeVarData.m_numGenericTypeLevels >= genericTypeDepth) &&
                        arguments.getTypeArguments().empty()) {
                        return true;
                    }
                }
                for (const auto& arg : arguments.getTypeArguments()) {
                    Visitor argVisitor{genericTypeDepth};
                    if (arg.visit<Visitor, bool>(argVisitor)) {
                        return true;
                    }
                }
                return false;
            }
            unsigned int m_genericTypeDepth;
        } visitor{0};
        return typeRef.visit<Visitor, bool>(visitor);
    }

    struct TypeVariableExplorer {
        TypeVariableExplorer(int maximumPossibleHeight)
            : m_maximumPossibleHeight(maximumPossibleHeight) {}

        int m_maximumPossibleHeight;
        int m_maximumHeightFound = -1;

        // Returns true if the exploration can be stopped.
        bool containsUnassignedTypeVariableImpl(const babelwires::ValueTreeNode& valueTreeNode,
                                                unsigned int genericTypeDepth) {
            const babelwires::Type& type = valueTreeNode.getType();
            if (type.as<babelwires::TypeVariableType>()) {
                // An unassigned type variable.
                const auto typeVarData = babelwires::TypeVariableData::isTypeVariable(valueTreeNode.getTypeRef());
                assert(typeVarData);
                const int excess = typeVarData->m_numGenericTypeLevels - genericTypeDepth;
                if (excess > m_maximumHeightFound) {
                    // The variable is unassigned and references a generic type above the start point of the search (0).
                    m_maximumHeightFound = excess;
                    if (m_maximumHeightFound >= m_maximumPossibleHeight) {
                        // The variable references the outermost generic type, which means we can stop exploring.
                        // (If it references a more nested generic type, then we wouldn't know whether nodes on the
                        // valueTree path between those types should be marked as having an unassigned type variable
                        // or not. In that case, we need to keep exploring.)
                        // We use >= and don't assert because a badly formed type might have been deserialized.
                        return true;
                    }
                }
            } else if (type.as<babelwires::GenericType>()) {
                ++genericTypeDepth;
            }
            for (int i = 0; i < valueTreeNode.getNumChildren(); ++i) {
                const babelwires::ValueTreeNode* const child = valueTreeNode.getChild(i);
                assert(child && "ValueTreeNode::getChild returned nullptr");
                if (containsUnassignedTypeVariableImpl(*child, genericTypeDepth)) {
                    // Stop exploring.
                    return true;
                }
            }
            return false;
        }
    };

} // namespace

bool babelwires::containsUnassignedTypeVariable(const ValueTreeNode& valueTreeNode) {
    int maximumHeight = getMaximumPossibleHeightOfUnassignedGenericType(valueTreeNode);
    if (maximumHeight < 0) {
        return false;
    }
    if (!typeRefContainsTypeVariable(valueTreeNode.getTypeRef())) {
        return false;
    }
    TypeVariableExplorer explorer(maximumHeight);
    explorer.containsUnassignedTypeVariableImpl(valueTreeNode, 0);
    return explorer.m_maximumHeightFound >= 0;
}

int babelwires::getMaximumHeightOfUnassignedGenericType(const ValueTreeNode& valueTreeNode, int maximumPossible) {
    assert(maximumPossible >= 0);
    int tmpMax = getMaximumPossibleHeightOfUnassignedGenericType(valueTreeNode);
    assert(maximumPossible == tmpMax);
    if (!typeRefContainsTypeVariable(valueTreeNode.getTypeRef())) {
        return -1;
    }
    TypeVariableExplorer explorer(maximumPossible);
    explorer.containsUnassignedTypeVariableImpl(valueTreeNode, 0);
    return explorer.m_maximumHeightFound;
}

namespace {
    struct TypeVariableAssignmentFinder {
        const babelwires::TypeSystem& m_typeSystem;
        const babelwires::ValueTreeNode& m_targetValueTreeRoot;
        std::map<std::tuple<babelwires::Path, unsigned int>, babelwires::TypeRef> m_assignments;

        /// Explore the source value _as if_ it was an element of the target type and simultaneously explore the source
        /// value in a normal way using the source type.
        // TODO: I shouldn't need to pass the source value twice here. Probably the API of visitValue should take a ValueHolder.
        bool findAssignments(const babelwires::TypeRef& targetTypeRef, const babelwires::Value& sourceAsTargetValue,
                             const babelwires::TypeRef& sourceTypeRef, const babelwires::ValueHolder& sourceValue,
                             const babelwires::Path& pathToCurrentNode) {
            if (auto typeVariableData = babelwires::TypeVariableData::isTypeVariable(targetTypeRef)) {
                return handleAssignment(*typeVariableData, targetTypeRef, pathToCurrentNode);
            }
            babelwires::Type::ChildValueVisitor childValueVisitor = [&](const babelwires::TypeSystem& typeSystem,
                                                                        const babelwires::TypeRef& childTypeRef,
                                                                        const babelwires::Value& childValue,
                                                                        const babelwires::PathStep& pathStep) {
                const babelwires::CompoundType* sourceCompound =
                    sourceTypeRef.resolve(typeSystem).as<babelwires::CompoundType>();
                if (!sourceCompound) {
                    return false;
                }
                const unsigned int childIndexInSourceType =
                    sourceCompound->getChildIndexFromStep(sourceValue, pathStep);
                const auto [sourceChildValuePtr, _, sourceChildTypeRef] =
                    sourceCompound->getChild(sourceValue, childIndexInSourceType);
                if (!sourceChildValuePtr) {
                    return false;
                }
                babelwires::Path pathToChild = pathToCurrentNode;
                pathToChild.pushStep(pathStep);
                return findAssignments(childTypeRef, childValue, sourceChildTypeRef, *sourceChildValuePtr, pathToChild);
            };
            const babelwires::Type& targetType = targetTypeRef.resolve(m_typeSystem);
            if (!targetType.visitValue(m_typeSystem, sourceAsTargetValue, childValueVisitor)) {
                return false;
            }
            return true;
        }

        bool handleAssignment(const babelwires::TypeVariableData& typeVariableData,
                              const babelwires::TypeRef& sourceTypeRef, const babelwires::Path& pathToCurrentNode) {
            // This should always succeed.
            const babelwires::ValueTreeNode& typeVariableNode = followPath(pathToCurrentNode, m_targetValueTreeRoot);
            // This may not, if the type is badly formed.
            const babelwires::ValueTreeNode* const genericTypeNodePtr = tryGetGenericTypeFromVariable(typeVariableNode);
            if (!genericTypeNodePtr) {
                // Don't assert when the type is badly formed.
                return false;
            }
            // Check whether this variable already had an assignment in the target tree.
            if (const babelwires::TypeRef& existingAssignment = genericTypeNodePtr->getType().is<babelwires::GenericType>().getTypeAssignment(
                genericTypeNodePtr->getValue(), typeVariableData.m_typeVariableIndex)) {
                switch (m_typeSystem.compareSubtype(sourceTypeRef, existingAssignment)) {
                    case babelwires::SubtypeOrder::IsEquivalent:
                    case babelwires::SubtypeOrder::IsSubtype:
                        // Existing assignment is more general than or equal to the new one: keep existing.
                        return true;
                    default:
                        // Never overwrite the existing assignment, so treat as a conflicting assignment.
                        return false;
                }
            }

            const babelwires::Path pathToGenericType = getPathTo(genericTypeNodePtr);
            const std::tuple<babelwires::Path, unsigned int> key{pathToGenericType,
                                                                   typeVariableData.m_typeVariableIndex};
            auto [it, inserted] = m_assignments.insert_or_assign(key, sourceTypeRef);
            if (inserted) {
                return true;
            } else {
                // An instance of this type variable may already be assigned by the exploration algorithm, so check for consistency.
                switch (m_typeSystem.compareSubtype(sourceTypeRef, it->second)) {
                    case babelwires::SubtypeOrder::IsEquivalent:
                        return true;
                    case babelwires::SubtypeOrder::IsSubtype:
                        // Existing assignment is more general than or equal to the new one: keep existing.
                        return true;
                    case babelwires::SubtypeOrder::IsSupertype:
                        // New assignment is more general than existing: update to new.
                        it->second = sourceTypeRef;
                        return true;
                    case babelwires::SubtypeOrder::IsIntersecting:
                    case babelwires::SubtypeOrder::IsDisjoint:
                    default:
                        // Treat as conflicting assignment.
                        return false;
                }
            }
        }
    };

} // namespace

std::optional<std::map<std::tuple<babelwires::Path, unsigned int>, babelwires::TypeRef>>
babelwires::getTypeVariableAssignments(const ValueTreeNode& sourceValueTreeNode,
                                       const ValueTreeNode& targetValueTreeNode) {
    assert(containsUnassignedTypeVariable(targetValueTreeNode) &&
            "Target ValueTreeNode has no unassigned type variables");
    const TypeSystem& typeSystem = sourceValueTreeNode.getTypeSystem();
    std::map<std::tuple<Path, unsigned int>, TypeRef> assignments;
    const TypeRef& targetTypeRef = targetValueTreeNode.getTypeRef();
    const TypeRef& sourceTypeRef = sourceValueTreeNode.getTypeRef();
    const ValueHolder& sourceValue = sourceValueTreeNode.getValue();
    const RootAndPath<const ValueTreeRoot> rootAndPath = getRootAndPathTo(targetValueTreeNode);

    TypeVariableAssignmentFinder finder{typeSystem, rootAndPath.m_root, assignments};
    // The source value is passed in twice here. See the comment on findAssignments.
    if (!finder.findAssignments(targetTypeRef, *sourceValue, sourceTypeRef, sourceValue, rootAndPath.m_pathFromRoot)) {
        return std::nullopt;
    }
    return assignments;
}
