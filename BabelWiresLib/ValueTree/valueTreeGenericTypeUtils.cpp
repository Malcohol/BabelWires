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
    bool isUnderGenericTypeWithUnresolvedVariables(const babelwires::ValueTreeNode& valueTreeNode) {
        const babelwires::ValueTreeNode* current = &valueTreeNode;
        const babelwires::ValueTreeNode* parent = current->getOwner();
        while (parent) {
            if (parent->getType().as<babelwires::GenericType>()) {
                const babelwires::GenericType& genericType = parent->getType().is<babelwires::GenericType>();
                for (unsigned int i = 0; i < genericType.getNumVariables(); ++i) {
                    if (genericType.getTypeAssignment(parent->getValue(), i) == babelwires::TypeRef()) {
                        return true;
                    }
                }
            }
            current = parent;
            parent = current->getOwner();
        }
        return false;
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
                        babelwires::TypeVariableTypeConstructor::extractValueArguments(arguments.m_valueArguments);
                    if ((typeVarData.m_numGenericTypeLevels >= genericTypeDepth) && arguments.m_typeArguments.empty()) {
                        return true;
                    }
                }
                for (const auto& arg : arguments.m_typeArguments) {
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

    bool containsUnresolvedTypeVariableImpl(const babelwires::ValueTreeNode& valueTreeNode,
                                            unsigned int genericTypeDepth = 0) {
        if (valueTreeNode.getType().as<babelwires::TypeVariableType>()) {
            const auto typeVarData = babelwires::TypeVariableData::isTypeVariable(valueTreeNode.getTypeRef());
            if (typeVarData && (typeVarData->m_numGenericTypeLevels >= genericTypeDepth)) {
                // The variable is unresolved and references a generic type above the start point of the search.
                return true;
            }
        }
        for (int i = 0; i < valueTreeNode.getNumChildren(); ++i) {
            const babelwires::ValueTreeNode* const child = valueTreeNode.getChild(i);
            assert(child && "ValueTreeNode::getChild returned nullptr");
            if (child->getType().as<babelwires::GenericType>()) {
                ++genericTypeDepth;
            } else if (auto typeVarData = babelwires::TypeVariableData::isTypeVariable(child->getTypeRef())) {
                if ((typeVarData->m_numGenericTypeLevels >= genericTypeDepth) &&
                    (child->getType().as<babelwires::TypeVariableType>())) {
                    // The variable is unresolved and references a generic type above the start point of the search.
                    return true;
                }
            }
            if (containsUnresolvedTypeVariableImpl(*child, genericTypeDepth)) {
                return true;
            }
        }
        return false;
    }

} // namespace

bool babelwires::containsUnresolvedTypeVariable(const ValueTreeNode& valueTreeNode) {
    // Two early out conditions to avoid doing a full traversal.
    if (!isUnderGenericTypeWithUnresolvedVariables(valueTreeNode)) {
        return false;
    }
    if (!typeRefContainsTypeVariable(valueTreeNode.getTypeRef())) {
        return false;
    }
    return containsUnresolvedTypeVariableImpl(valueTreeNode, 0);
}
