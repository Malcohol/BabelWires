/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

const babelwires::ValueTreeNode* babelwires::tryGetGenericTypeFromVariable(const ValueTreeNode& valueTreeNode) {
    auto variableData = TypeVariableData::isTypeVariable(valueTreeNode.getTypeRef());
    assert(variableData && "ValueTreeNode is not a type variable");

    unsigned int level = variableData->m_numGenericTypeLevels;
    const ValueTreeNode* current = &valueTreeNode;
    const ValueTreeNode* parent;
    do {
        parent = current->getOwner();
        if (!parent) {
            // This could only happen if the type is badly formed.
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

    unsigned int typeRefContainsUnassignedTypeVariable(const babelwires::TypeRef& typeRef, int earlyOutHeight) {
        int maximumHeightFound = -1;
        // Return true to stop exploring.
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

                    const int excess = typeVarData.m_numGenericTypeLevels - genericTypeDepth;
                    if ((excess > m_maximumHeightFound) && arguments.getTypeArguments().empty()) {
                        // The variable is unassigned and references a generic type above the start point of the search
                        // (0).
                        m_maximumHeightFound = excess;
                        if (m_maximumHeightFound >= m_earlyOutHeight) {
                            // The variable references the outermost generic type, which means we can stop exploring.
                            // (If it references a more nested generic type, then we wouldn't know whether nodes on the
                            // valueTree path between those types should be marked as having an unassigned type variable
                            // or not. In that case, we need to keep exploring.)
                            // We use >= and don't assert because a badly formed type might have been deserialized.
                            return true;
                        }
                    }
                }
                for (const auto& arg : arguments.getTypeArguments()) {
                    Visitor argVisitor{m_earlyOutHeight, m_maximumHeightFound, genericTypeDepth};
                    if (arg.visit<Visitor, bool>(argVisitor)) {
                        return true;
                    }
                }
                return false;
            }
            int m_earlyOutHeight;
            int& m_maximumHeightFound;
            unsigned int m_genericTypeDepth = 0;
        } visitor{earlyOutHeight, maximumHeightFound};
        typeRef.visit<Visitor, bool>(visitor);
        return maximumHeightFound;
    }
} // namespace

bool babelwires::containsUnassignedTypeVariable(const ValueTreeNode& valueTreeNode) {
    int maximumHeight = getMaximumPossibleHeightOfUnassignedGenericType(valueTreeNode);
    if (maximumHeight < 0) {
        return false;
    }
    const int maximumHeightFound = typeRefContainsUnassignedTypeVariable(valueTreeNode.getTypeRef(), 0);
    return maximumHeightFound >= 0;
}

int babelwires::getMaximumHeightOfUnassignedGenericType(const ValueTreeNode& valueTreeNode, int maximumPossible) {
    assert(maximumPossible >= 0);
#ifndef NDEBUG
    const int tmpMax = getMaximumPossibleHeightOfUnassignedGenericType(valueTreeNode);
    assert(maximumPossible == tmpMax);
#endif
    return typeRefContainsUnassignedTypeVariable(valueTreeNode.getTypeRef(), maximumPossible);
}

namespace {
    struct TypeVariableAssignmentFinder {
        const babelwires::TypeSystem& m_typeSystem;
        const babelwires::ValueTreeNode& m_targetNode;
        // The nodes with generic types between the targetNode and the root (in targetNode-to-root order).
        std::vector<const babelwires::ValueTreeNode*> m_genericNodes;
        std::map<std::tuple<babelwires::Path, unsigned int>, babelwires::TypeRef> m_assignments;

        TypeVariableAssignmentFinder(const babelwires::TypeSystem& typeSystem,
                                     const babelwires::ValueTreeNode& targetNode)
            : m_typeSystem(typeSystem)
            , m_targetNode(targetNode) {

            // Precompute the paths to all generic types between the start node and the root.
            const babelwires::ValueTreeNode* current = targetNode.getOwner();
            while (current) {
                if (current->getType().as<babelwires::GenericType>()) {
                    m_genericNodes.push_back(current);
                }
                current = current->getOwner();
            }
        }

        /// Explore the source value _as if_ it was an element of the target type and simultaneously explore the source
        /// value in a normal way using the source type.
        /// The algorithm is only concerned with finding assignments to type variables in the target type that reference
        /// generic types wrapping the target value tree node.
        /// The extraGenericTypeDepth counts how many generic types have been encountered _within_ the targetType (type
        /// variables might reference these, but they do not get type assignments from this algorithm).
        bool findAssignments(const babelwires::TypeRef& targetTypeRef, const babelwires::TypeRef& sourceTypeRef,
                             const babelwires::ValueHolder& sourceValue, unsigned int extraGenericTypeDepth = 0) {
            if (auto typeVariableData = babelwires::TypeVariableData::isTypeVariable(targetTypeRef)) {
                return handleAssignment(*typeVariableData, sourceTypeRef, extraGenericTypeDepth);
            }
            babelwires::Type::ChildValueVisitor childValueVisitor = [&](const babelwires::TypeSystem& typeSystem,
                                                                        const babelwires::TypeRef& childTypeRef,
                                                                        const babelwires::Value& childValue,
                                                                        const babelwires::PathStep& pathStep) {
                const auto& sourceCompound =
                    sourceTypeRef.resolveAs<babelwires::CompoundType>(typeSystem);
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
                const babelwires::TypePtr& childType = childTypeRef.resolve(typeSystem);
                if (childType->as<babelwires::GenericType>()) {
                    ++extraGenericTypeDepth;
                }
                return findAssignments(childTypeRef, sourceChildTypeRef, *sourceChildValuePtr, extraGenericTypeDepth);
            };
            const babelwires::TypePtr& targetType = targetTypeRef.resolve(m_typeSystem);
            if (!targetType->visitValue(m_typeSystem, *sourceValue, childValueVisitor)) {
                return false;
            }
            return true;
        }

        bool handleAssignment(const babelwires::TypeVariableData& typeVariableData,
                              const babelwires::TypeRef& sourceTypeRef, unsigned int extraGenericTypeDepth) {
            const int excessGenericTypeDepth =
                static_cast<int>(typeVariableData.m_numGenericTypeLevels) - static_cast<int>(extraGenericTypeDepth);
            if (excessGenericTypeDepth < 0) {
                // The type variable references a generic type below the start point of the search, so does not get
                // assigned.
                return true;
            }
            if (excessGenericTypeDepth >= static_cast<int>(m_genericNodes.size())) {
                // This could happen if the type is badly formed.
                return false;
            }
            const babelwires::ValueTreeNode* const genericTypeNodePtr = m_genericNodes[excessGenericTypeDepth];
            assert(genericTypeNodePtr);
            // Check whether this variable already had an assignment in the target tree.
            if (const babelwires::TypeRef& existingAssignment =
                    genericTypeNodePtr->getType().is<babelwires::GenericType>().getTypeAssignment(
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
                // An instance of this type variable may already be assigned by the exploration algorithm, so check for
                // consistency.
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
    const TypeRef& targetTypeRef = targetValueTreeNode.getTypeRef();
    const TypeRef& sourceTypeRef = sourceValueTreeNode.getTypeRef();
    const ValueHolder& sourceValue = sourceValueTreeNode.getValue();

    TypeVariableAssignmentFinder finder{typeSystem, targetValueTreeNode};
    // The source value is passed in twice here. See the comment on findAssignments.
    if (!finder.findAssignments(targetTypeRef, sourceTypeRef, sourceValue)) {
        return std::nullopt;
    }
    return finder.m_assignments;
}
