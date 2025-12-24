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

// Algorithm for matching a compound type containing a type variables against a source type and value.
// It might be sufficient to match any type variable and allow the type system to accept or reject
// the assignment using its usual rules.

// Let's imagine that isValidValue was reimplemented using visitValue:
//
// using ChildValueVisitor = std::function<bool(const TypeSystem& typeSystem, const TypeRef& childTypeRef, const PathStep& pathStep, const
// const ValueHolder& v)>;
//
// Type::visitValue(const TypeSystem& typeSystem, const ValueHolder& v, ChildValueVisitor& visitor) const
//
// bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override {
//     ValueVisitor visitor = [&](const TypeRef& childTypeRef, const ValueHolder& v) {
//         const Type& childType = childTypeRef.resolve(typeSystem);
//         return childType.isValidValue(typeSystem, v);
//     };
//     return this->visitValue(typeSystem, v, visitor);

// bool babelwires::matchTypeWithTypeVariables(const TypeSystem& typeSystem, const TypeRef& genericTypeRef, const TypeRef& inTypeRef,
//                                          const ValueHolder& inValue,
//   std::map<unsigned int, TypeRef>& outTypeVariableAssignments) {
//      ValueVisitor visitor = [&](const TypeSystem& typeSystem, const TypeRef& childTypeRef, const PathStep& pathStep, const
// const ValueHolder& v) {
//      const Type& childType = childTypeRef.resolve(typeSystem);
//      if (const auto* typeVariableType = childType.as<TypeVariableType>()) {
//          const auto typeVarData = typeVariableType->getTypeVariableData();
//          // Handle the type variable assignment
//      else {
//          const unsigned int childIndexInSourceType = inType.getChildIndexFromStep(typeSystem, pathStep);
//          const auto [sourceChildValuePtr, , sourceChildTypeRef] = inType.getChild(inValue, childIndexInSourceType);
//          if (!sourceChildValuePtr) {
//              return false;
//          }
//          return matchTypeWithTypeVariables(typeSystem, childTypeRef, sourceChildTypeRef, *sourceChildValuePtr, outTypeVariableAssignments);
//      }
//      const Type& genericType = genericTypeRef.resolve(typeSystem);
//      if (!genericType.visitValue(typeSystem, genericValue, visitor)) {
//          return false;
//      }
//      return true;
//}


