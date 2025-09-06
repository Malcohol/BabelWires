/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

babelwires::Path babelwires::getPathTo(const ValueTreeNode* valueTreeNode) {
    std::vector<PathStep> steps;
    const ValueTreeNode* current = valueTreeNode;
    const ValueTreeNode* parent = current->getOwner();
    while (parent) {
        steps.push_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() &&
               "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwner();
    }
    std::reverse(steps.begin(), steps.end());
    return Path(std::move(steps));
}

babelwires::RootAndPath<const babelwires::ValueTreeRoot>
babelwires::getRootAndPathTo(const ValueTreeNode& valueTreeNode) {
    std::vector<PathStep> steps;
    const ValueTreeNode* current = &valueTreeNode;
    const ValueTreeNode* parent = valueTreeNode.getOwner();
    while (parent) {
        steps.emplace_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() &&
               "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwner();
    }
    std::reverse(steps.begin(), steps.end());
    return {current->is<ValueTreeRoot>(), Path(std::move(steps))};
}

babelwires::RootAndPath<babelwires::ValueTreeRoot> babelwires::getRootAndPathTo(ValueTreeNode& valueTreeNode) {
    std::vector<PathStep> steps;
    ValueTreeNode* current = &valueTreeNode;
    ValueTreeNode* parent = valueTreeNode.getOwnerNonConst();
    while (parent) {
        steps.emplace_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() &&
               "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwnerNonConst();
    }
    std::reverse(steps.begin(), steps.end());
    return {current->is<ValueTreeRoot>(), Path(std::move(steps))};
}

namespace {

    template <typename T> T& followPathImpl(T& start, const babelwires::Path& p, int& index) {
        if (index < p.getNumSteps()) {
            T& child = start.getChildFromStep(p.getStep(index));
            ++index;
            return followPathImpl(child, p, index);
        } else {
            return start;
        }
    }

    template <typename T> T& followPathImpl(T& start, const babelwires::Path& p) {
        int index = 0;
        try {
            return followPathImpl(start, p, index);
        } catch (const std::exception& e) {
            throw babelwires::ModelException()
                << e.what() << "; when trying to follow step #" << index + 1 << " in path \"" << p << '\"';
        }
    }

} // namespace

babelwires::ValueTreeNode& babelwires::followPath(const Path& path, ValueTreeNode& start) {
    return followPathImpl<ValueTreeNode>(start, path);
}

const babelwires::ValueTreeNode& babelwires::followPath(const Path& path, const ValueTreeNode& start) {
    return followPathImpl<const ValueTreeNode>(start, path);
}

namespace {

    template <typename T> T* tryFollowPathImpl(T* start, const babelwires::Path& p, int index = 0) {
        if (start && index < p.getNumSteps()) {
            T* child = start->tryGetChildFromStep(p.getStep(index));
            return tryFollowPathImpl(child, p, index + 1);
        } else {
            return start;
        }
    }

} // namespace

babelwires::ValueTreeNode* babelwires::tryFollowPath(const Path& path, ValueTreeNode& start) {
    return tryFollowPathImpl<ValueTreeNode>(&start, path);
}

const babelwires::ValueTreeNode* babelwires::tryFollowPath(const Path& path, const ValueTreeNode& start) {
    return tryFollowPathImpl<const ValueTreeNode>(&start, path);
}

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
