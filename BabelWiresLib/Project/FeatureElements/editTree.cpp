/**
 * The EditTree arranges edits (modifiers and expand/collapse) in a tree organized by paths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <cassert>
#include <optional>

inline void babelwires::EditTree::RootedPathIterator::operator++() {
    if (m_isAtRoot) {
        m_isAtRoot = false;
    } else {
        ++m_it;
    }
}

inline bool babelwires::EditTree::RootedPathIterator::operator==(const RootedPathIterator& other) const {
    return (m_it == other.m_it) && (m_isAtRoot == other.m_isAtRoot);
}

inline bool babelwires::EditTree::RootedPathIterator::operator!=(const RootedPathIterator& other) const {
    return !(*this == other);
};

inline int babelwires::EditTree::RootedPathIterator::distanceFrom(const RootedPathIterator& other) const {
    int itDist = m_it - other.m_it;
    if (m_isAtRoot) {
        --itDist;
    }
    if (other.m_isAtRoot) {
        ++itDist;
    }
    return itDist;
}

inline babelwires::PathStep babelwires::EditTree::RootedPathIterator::operator*() const {
    if (m_isAtRoot) {
        return {};
    } else {
        return *m_it;
    }
}

inline babelwires::EditTree::RootedPath::RootedPath(const Path& path)
    : m_path(path) {}

inline babelwires::EditTree::RootedPathIterator babelwires::EditTree::RootedPath::begin() const {
    return RootedPathIterator{m_path.begin(), true};
}
inline babelwires::EditTree::RootedPathIterator babelwires::EditTree::RootedPath::end() const {
    return RootedPathIterator{m_path.end(), false};
};

inline unsigned int babelwires::EditTree::RootedPath::getNumSteps() const {
    return m_path.getNumSteps() + 1;
}

babelwires::EditTree::~EditTree() = default;

babelwires::EditTree::FindNodeIndexResult babelwires::EditTree::findNodeIndex(RootedPathIterator& current,
                                                                              const RootedPathIterator& end,
                                                                              AncestorStack* ancestorStackOut) const {
    if (m_nodes.empty()) {
        return {-1, 0};
    }

    int nodeIndex = -1;
    int endOfChildren = m_nodes.size();

    while (current != end) {
        int childIndex = nodeIndex + 1;
        while (childIndex < endOfChildren) {
            const TreeNode& child = m_nodes[childIndex];
            if (child.m_step == *current) {
                ++current;
                nodeIndex = childIndex;
                endOfChildren = nodeIndex + child.m_numDescendents + 1;
                if (ancestorStackOut) {
                    ancestorStackOut->emplace_back(nodeIndex);
                }
                break;
            } else if (*current < child.m_step) {
                // A new child would be inserted where this one is.
                return {nodeIndex, childIndex};
            }
            childIndex += child.m_numDescendents + 1;
        }
        assert(childIndex <= endOfChildren);
        if (childIndex == endOfChildren) {
            // A new child will occur after the current ones.
            return {nodeIndex, childIndex};
        }
    }

    // We found the node, no missing child.
    return {nodeIndex, -1};
}

bool babelwires::EditTree::TreeNode::isNeeded() const {
    return m_modifier || (m_isExpanded != c_expandedByDefault) || (m_isImplicitlyExpanded != c_expandedByDefault) ||
           m_isExpandedChanged || (m_numDescendents > 0);
}

void babelwires::EditTree::addEdit(const Path& path, const EditNodeFunc& applyFunc) {
    RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();
    AncestorStack ancestorStack;
    ancestorStack.reserve(16);
    auto [nodeIndex, childIndex] = findNodeIndex(it, end, &ancestorStack);
    const auto numMissingNodes = end.distanceFrom(it);
    if (numMissingNodes > 0) {
        const auto oldSize = m_nodes.size();
        m_nodes.resize(m_nodes.size() + numMissingNodes);
        std::move_backward(m_nodes.begin() + childIndex, m_nodes.begin() + oldSize, m_nodes.end());
        for (int i = 0; i < numMissingNodes; ++i) {
            TreeNode& newNode = m_nodes[childIndex + i];
            newNode = TreeNode();
            newNode.m_step = *it;
            newNode.m_numDescendents = numMissingNodes - i - 1;
            ++it;
        }
        for (auto ancestorIndex : ancestorStack) {
            m_nodes[ancestorIndex].m_numDescendents += numMissingNodes;
        }
        nodeIndex = childIndex + numMissingNodes - 1;
    }
    TreeNode& nodeToEdit = m_nodes[nodeIndex];
    applyFunc(nodeToEdit);
    assert(validateTree());
}

void babelwires::EditTree::removeEdit(const Path& path, const EditNodeFunc& applyFunc) {
    RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();
    AncestorStack ancestorStack;
    ancestorStack.reserve(16);
    const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end(), &ancestorStack);
    assert((it == end) && "The expected edit was not in the tree");
    {
        TreeNode& nodeToEdit = m_nodes[nodeIndex];
        applyFunc(nodeToEdit);
    }

    unsigned int numToRemove = 0;
    for (int i = ancestorStack.size() - 1; i >= 0; --i) {
        TreeNode& node = m_nodes[ancestorStack[i]];
        node.m_numDescendents -= numToRemove;
        if (!node.isNeeded()) {
            ++numToRemove;
        }
    }

    if (numToRemove > 0) {
        const int firstStackIndexToRemove = ancestorStack.size() - numToRemove;
#ifndef NDEBUG
        // Confirm that the nodes to remove are contiguous.
        for (unsigned int i = firstStackIndexToRemove; i < ancestorStack.size() - 1; ++i) {
            assert(ancestorStack[i] + 1 == ancestorStack[i + 1]);
        }
#endif
        auto firstNodeToRemove = m_nodes.begin() + ancestorStack[firstStackIndexToRemove];
        m_nodes.erase(firstNodeToRemove, firstNodeToRemove + numToRemove);
    }
    assert(validateTree());
}

void babelwires::EditTree::addModifier(std::unique_ptr<Modifier> modifier) {
    const Path& featurePath = modifier->getPathToModify();

    addEdit(featurePath, [&modifier](TreeNode& nodeToEdit) {
        assert(!nodeToEdit.m_modifier.get() && "There's already a modifier at that path in the tree");
        nodeToEdit.m_modifier = std::move(modifier);
    });
}

std::unique_ptr<babelwires::Modifier> babelwires::EditTree::removeModifier(const Modifier* modifier) {
    const Path& featurePath = modifier->getPathToModify();
    std::unique_ptr<babelwires::Modifier> result;

    removeEdit(featurePath, [modifier, &result](TreeNode& nodeToEdit) {
        assert((nodeToEdit.m_modifier.get() == modifier) &&
               "A different modifier was in the tree at the modifier's path");
        // Clearing the modifier so it no longer makes the node appear needed.
        result = std::move(nodeToEdit.m_modifier);
    });

    return result;
}

babelwires::Modifier* babelwires::EditTree::findModifier(const Path& path) {
    const RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();
    const auto [nodeIndex, _] = findNodeIndex(it, end);
    if ((nodeIndex == -1) || (it != end)) {
        return nullptr;
    } else {
        return m_nodes[nodeIndex].m_modifier.get();
    }
}

const babelwires::Modifier* babelwires::EditTree::findModifier(const Path& path) const {
    const RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();
    const auto [nodeIndex, _] = findNodeIndex(it, end);
    if ((nodeIndex == -1) || (it != end)) {
        return nullptr;
    } else {
        return m_nodes[nodeIndex].m_modifier.get();
    }
}

void babelwires::EditTree::adjustArrayIndices(const babelwires::Path& path, babelwires::ArrayIndex startIndex,
                                              int adjustment) {
    const RootedPath pathToArray{path};
    auto it = pathToArray.begin();
    const auto end = pathToArray.end();

    const auto [nodeIndex, _] = findNodeIndex(it, end);
    if ((nodeIndex == -1) || (it != pathToArray.end())) {
        return;
    }

    const TreeNode& node = m_nodes[nodeIndex];
    int endOfChildren = nodeIndex + node.m_numDescendents + 1;

    /// We have to be very careful about the m_isExpandedChanged values, which must be unaffected by this operation.
    /// Thus we use the conventional API to add/remove modifiers and setExpanded, rather than adjust them in-place.
    /// This does not cause the modifiers to appear add/removed outside the edit tree.
    std::vector<Modifier*> modifiersToAdjust;
    std::vector<Path> pathsToToggleExpansion;

    int childIndex = nodeIndex + 1;
    while (childIndex < endOfChildren) {
        const TreeNode& child = m_nodes[childIndex];
        if (child.m_step.isIndex()) {
            ArrayIndex childArrayIndex = child.m_step.getIndex();
            if (childArrayIndex >= startIndex) {
                // If there are modifiers in the subtree at this child, adjust them.
                for (ArrayIndex di = childIndex; di <= childIndex + child.m_numDescendents; ++di) {
                    const TreeNode& descendent = m_nodes[di];
                    if (descendent.m_modifier) {
                        modifiersToAdjust.emplace_back(descendent.m_modifier.get());
                    }
                    if (descendent.m_isExpanded != c_expandedByDefault) {
                        pathsToToggleExpansion.emplace_back(getPathToNode(di));
                    }
                }
            }
        }
        childIndex += child.m_numDescendents + 1;
    }

    {
        std::vector<std::unique_ptr<Modifier>> modifiersAdjusted;

        for (const auto& m : modifiersToAdjust) {
            auto modPtr = removeModifier(m);
            modPtr->adjustArrayIndex(pathToArray.m_path, startIndex, adjustment);
            modifiersAdjusted.emplace_back(std::move(modPtr));
        }
        for (auto& m : modifiersAdjusted) {
            addModifier(std::move(m));
        }
    }

    for (const auto& p : pathsToToggleExpansion) {
        setExpanded(p, c_expandedByDefault);
    }

    const unsigned int pathIndexOfStepIntoArray = path.getNumSteps();

    for (auto p : pathsToToggleExpansion) {
        assert(pathToArray.m_path.isStrictPrefixOf(p));
        babelwires::PathStep& stepIntoArray = p.getStep(pathIndexOfStepIntoArray);
        auto index = stepIntoArray.getIndex();
        assert((index >= startIndex) && "This code only applies when the index is correct.");
        stepIntoArray = index + adjustment;
        setExpanded(p, !c_expandedByDefault);
    }
}

bool babelwires::EditTree::isExpanded(const Path& path) const {
    const RootedPath pathToArray(path);
    auto it = pathToArray.begin();
    const auto end = pathToArray.end();

    const auto [nodeIndex, _] = findNodeIndex(it, end);
    if ((nodeIndex == -1) || (it != end)) {
        return c_expandedByDefault;
    } else {
        return m_nodes[nodeIndex].m_isExpanded || m_nodes[nodeIndex].m_isImplicitlyExpanded;
    }
}

void babelwires::EditTree::setExpanded(const Path& featurePath, bool expanded) {
    const auto toggleIsExpanded = [expanded](TreeNode& nodeToEdit) {
        assert((nodeToEdit.m_isExpanded != expanded) &&
               "The tree node was already in the expanded state we're trying to set");
        nodeToEdit.m_isExpanded = expanded;
        // Some algorithms cause (e.g. array removal) can cause this m_isExanded to be changed
        // and changed back, in which case it should be treated as not changed.
        nodeToEdit.m_isExpandedChanged = !nodeToEdit.m_isExpandedChanged;
    };

    if (expanded != c_expandedByDefault) {
        addEdit(featurePath, toggleIsExpanded);
    } else {
        removeEdit(featurePath, toggleIsExpanded);
    }
}

void babelwires::EditTree::setImplicitlyExpanded(const Path& featurePath, bool expanded) {
    const auto toggleIsImplicitlyExpanded = [expanded](TreeNode& nodeToEdit) {
        nodeToEdit.m_isImplicitlyExpanded = expanded;
    };

    assert((expanded != c_expandedByDefault) &&
           "Implicit expansion is expected to be the opposite of default expansion");
    addEdit(featurePath, toggleIsImplicitlyExpanded);
}

bool babelwires::EditTree::validateTree() const {
    if (m_nodes.size() == 0) {
        return true;
    }

    assert(m_nodes[0].m_step.isNotAStep());
    assert(m_nodes.size() == m_nodes[0].m_numDescendents + 1);

    std::vector<int> endOfChildrenStack;
    endOfChildrenStack.emplace_back(m_nodes.size());

    Path path;
    Path previousPath;

    for (int i = 1; i < m_nodes.size(); ++i) {
        assert(i <= endOfChildrenStack.back() && "Structural error in tree");
        const TreeNode& node = m_nodes[i];
        assert(node.isNeeded() && "Unnecessary node in the tree");

        while (i == endOfChildrenStack.back()) {
            endOfChildrenStack.pop_back();
            path.popStep();
        }

        path.pushStep(node.m_step);
        const int endOfChildren = i + node.m_numDescendents + 1;
        assert((endOfChildren <= endOfChildrenStack.back()) && "Descendents not scoped by parents");
        endOfChildrenStack.emplace_back(endOfChildren);

        assert((previousPath < path) && "Tree not in path order");
        previousPath = path;

        if (node.m_modifier) {
            assert((node.m_modifier->getPathToModify() == path) && "Node with wrong path");
        }
    }
    for (auto endOfChildren : endOfChildrenStack) {
        assert((endOfChildren == m_nodes.size()) && "Ragged ends");
    }
    assert(path.getNumSteps() == endOfChildrenStack.size() - 1);

    return true;
}

babelwires::Path babelwires::EditTree::getPathToNode(TreeNodeIndex soughtIndex) const {
    assert((soughtIndex < m_nodes.size()) && "soughtIndex is out of range");
    Path path;
    if (soughtIndex != 0) {
        int currentIndex = 1;

        while (currentIndex <= soughtIndex) {
            const int endOfChildren = currentIndex + m_nodes[currentIndex].m_numDescendents + 1;
            if (soughtIndex < endOfChildren) {
                path.pushStep(m_nodes[currentIndex].m_step);
                ++currentIndex;
            } else {
                currentIndex = endOfChildren;
            }
        }
    }
    return path;
}

void babelwires::EditTree::clearChanges() {
    // Since clearing a change can make nodes unnecessary, we use removeEdit to perform the change.
    // This approach is not optimal.
    std::vector<Path> pathsWithExpansionChanges;

    for (int i = 0; i < m_nodes.size(); ++i) {
        TreeNode& node = m_nodes[i];

        if (node.m_isExpandedChanged) {
            pathsWithExpansionChanges.emplace_back(getPathToNode(i));
        }
        if (node.m_modifier) {
            // This will never make a node unnecessary, so we clear the change directly.
            node.m_modifier->clearChanges();
        }
    }

    for (const Path& path : pathsWithExpansionChanges) {
        removeEdit(path, [](TreeNode& nodeToEdit) {
            assert(nodeToEdit.m_isExpandedChanged && "Should not be considering this node.");
            nodeToEdit.m_isExpandedChanged = false;
        });
    }
    assert(validateTree());
}

void babelwires::EditTree::truncatePathAtFirstCollapsedNode(babelwires::Path& path, State state) const {
    if (m_nodes.empty()) {
        // Path is not collapsed.
        if constexpr (!c_expandedByDefault) {
            path.truncate(0);
        }
        return;
    }

    RootedPath rootedPath(path);
    auto current = rootedPath.begin();
    const auto end = rootedPath.end();
    int pathIndex = 0;

    int nodeIndex = -1;
    int endOfChildren = m_nodes.size();

    while (current != end) {
        int childIndex = nodeIndex + 1;
        while (childIndex < endOfChildren) {
            const TreeNode& child = m_nodes[childIndex];
            if (child.m_step == *current) {
                if (!child.m_isImplicitlyExpanded &&
                    (((state == State::CurrentState) && !child.m_isExpanded) ||
                     ((state == State::PreviousState) && (child.m_isExpanded == child.m_isExpandedChanged)))) {
                    path.truncate(pathIndex);
                    return;
                }
                ++current;
                ++pathIndex;
                nodeIndex = childIndex;
                endOfChildren = nodeIndex + child.m_numDescendents + 1;
                break;
            } else if (*current < child.m_step) {
                // The path leads outside the tree.
                if constexpr (!c_expandedByDefault) {
                    path.truncate(pathIndex);
                }
                return;
            }
            childIndex += child.m_numDescendents + 1;
        }
        assert(childIndex <= endOfChildren);
        if (childIndex == endOfChildren) {
            // The path leads outside the tree.
            if constexpr (!c_expandedByDefault) {
                path.truncate(pathIndex);
            }
            return;
        }
    }
}

std::vector<babelwires::Path>
babelwires::EditTree::getAllExplicitlyExpandedPaths(const Path& path) const {
    std::vector<Path> expandedPaths;

    const RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();

    int beginIndex = 0;
    int endIndex = m_nodes.size();
    if (featurePath.getNumSteps() != 0) {
        auto it = featurePath.begin();
        const auto [nodeIndex, _] = findNodeIndex(it, end);
        if ((nodeIndex == -1) || (it != featurePath.end())) {
            return expandedPaths;
        }
        beginIndex = nodeIndex;
        endIndex = nodeIndex + m_nodes[nodeIndex].m_numDescendents + 1;
    }

    for (int i = beginIndex; i < endIndex; ++i) {
        if (m_nodes[i].m_isExpanded) {
            expandedPaths.emplace_back(getPathToNode(i));
        }
    }
    return expandedPaths;
}

babelwires::EditTree::Iterator<const babelwires::EditTree> babelwires::EditTree::begin() const {
    return {*this, 0, static_cast<TreeNodeIndex>(m_nodes.size())};
}

babelwires::EditTree::Iterator<babelwires::EditTree> babelwires::EditTree::begin() {
    return {*this, 0, static_cast<TreeNodeIndex>(m_nodes.size())};
}

babelwires::EditTree::Iterator<const babelwires::EditTree> babelwires::EditTree::end() const {
    return {*this, static_cast<TreeNodeIndex>(m_nodes.size()), static_cast<TreeNodeIndex>(m_nodes.size())};
}

babelwires::EditTree::Iterator<babelwires::EditTree> babelwires::EditTree::end() {
    return {*this, static_cast<TreeNodeIndex>(m_nodes.size()), static_cast<TreeNodeIndex>(m_nodes.size())};
}
