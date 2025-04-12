/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

babelwires::Path babelwires::getPathTo(const ValueTreeNode* valueTreeNode) {
    std::vector<PathStep> steps;
    const ValueTreeNode* current = valueTreeNode;
    const ValueTreeNode* parent = current->getOwner();
    while (parent) {
        steps.push_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() && "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwner();
    }
    std::reverse(steps.begin(), steps.end());
    return Path(std::move(steps));
}

babelwires::RootAndPath<const babelwires::ValueTreeRoot> babelwires::getRootAndPathTo(const ValueTreeNode& valueTreeNode) {
    std::vector<PathStep> steps;
    const ValueTreeNode* current = &valueTreeNode;
    const ValueTreeNode* parent = valueTreeNode.getOwner();
    while (parent) {
        steps.emplace_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() && "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwner();
    }
    std::reverse(steps.begin(), steps.end());
    return { current->is<ValueTreeRoot>(), Path(std::move(steps)) };
}

babelwires::RootAndPath<babelwires::ValueTreeRoot> babelwires::getRootAndPathTo(ValueTreeNode& valueTreeNode) {
    std::vector<PathStep> steps;
    ValueTreeNode* current = &valueTreeNode;
    ValueTreeNode* parent = valueTreeNode.getOwnerNonConst();
    while (parent) {
        steps.emplace_back(parent->getStepToChild(current));
        assert(!steps.back().isNotAStep() && "ValueTreeNode with a parent and whose step from that parent is not a step");
        current = parent;
        parent = current->getOwnerNonConst();
    }
    std::reverse(steps.begin(), steps.end());
    return { current->is<ValueTreeRoot>(), Path(std::move(steps)) };
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
