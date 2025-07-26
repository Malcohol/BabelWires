/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
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

babelwires::NodeAndPath<const babelwires::ValueTreeRoot>
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

babelwires::NodeAndPath<babelwires::ValueTreeRoot> babelwires::getRootAndPathTo(ValueTreeNode& valueTreeNode) {
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

const babelwires::ValueTreeNode*
babelwires::tryGetGenericTypeFromVariable(const ValueTreeNode& valueTreeNode) {
    auto variableData = TypeVariableTypeConstructor::isTypeVariable(valueTreeNode.getTypeRef());
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
