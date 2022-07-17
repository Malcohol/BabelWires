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

babelwires::EditTree::~EditTree() = default;

babelwires::EditTree::FindNodeIndexResult
babelwires::EditTree::findNodeIndex(babelwires::FeaturePath::const_iterator& current,
                                    const babelwires::FeaturePath::const_iterator& end,
                                    AncestorStack* ancestorStackOut) const {
    if (m_nodes.empty()) {
        return {-1, 0};
    }

    int nodeIndex = -1;
    int endOfChildren = m_nodes.size();

    while (current < end) {
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
    return m_modifier || (m_isExpanded != c_expandedByDefault) || m_isExpandedChanged || (m_numDescendents > 0);
}

void babelwires::EditTree::addEdit(const FeaturePath& featurePath, const EditNodeFunc& applyFunc) {
    assert((featurePath.getNumSteps() > 0) && "The root cannot carry edits");
    auto it = featurePath.begin();
    AncestorStack ancestorStack;
    ancestorStack.reserve(16);
    auto [nodeIndex, childIndex] = findNodeIndex(it, featurePath.end(), &ancestorStack);
    const auto numMissingNodes = std::distance(it, featurePath.end());
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

void babelwires::EditTree::removeEdit(const FeaturePath& featurePath, const EditNodeFunc& applyFunc) {
    auto it = featurePath.begin();
    AncestorStack ancestorStack;
    ancestorStack.reserve(16);
    const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end(), &ancestorStack);
    assert((it == featurePath.end()) && "The expected edit was not in the tree");
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
    const FeaturePath& featurePath = modifier->getPathToFeature();

    addEdit(featurePath, [&modifier](TreeNode& nodeToEdit) {
        assert(!nodeToEdit.m_modifier.get() && "There's already a modifier at that path in the tree");
        nodeToEdit.m_modifier = std::move(modifier);
    });
}

std::unique_ptr<babelwires::Modifier> babelwires::EditTree::removeModifier(const Modifier* modifier) {
    const FeaturePath& featurePath = modifier->getPathToFeature();
    std::unique_ptr<babelwires::Modifier> result;

    removeEdit(featurePath, [modifier, &result](TreeNode& nodeToEdit) {
        assert((nodeToEdit.m_modifier.get() == modifier) &&
               "A different modifier was in the tree at the modifier's path");
        // Clearing the modifier so it no longer makes the node appear needed.
        result = std::move(nodeToEdit.m_modifier);
    });

    return result;
}

babelwires::Modifier* babelwires::EditTree::findModifier(const FeaturePath& featurePath) {
    auto it = featurePath.begin();
    const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end());
    if ((nodeIndex == -1) || (it != featurePath.end())) {
        return nullptr;
    } else {
        return m_nodes[nodeIndex].m_modifier.get();
    }
}

const babelwires::Modifier* babelwires::EditTree::findModifier(const FeaturePath& featurePath) const {
    auto it = featurePath.begin();
    const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end());
    if ((nodeIndex == -1) || (it != featurePath.end())) {
        return nullptr;
    } else {
        return m_nodes[nodeIndex].m_modifier.get();
    }
}

void babelwires::EditTree::adjustArrayIndices(const babelwires::FeaturePath& pathToArray,
                                              babelwires::ArrayIndex startIndex, int adjustment) {
    auto it = pathToArray.begin();
    const auto [nodeIndex, _] = findNodeIndex(it, pathToArray.end());
    if ((nodeIndex == -1) || (it != pathToArray.end())) {
        return;
    }

    const TreeNode& node = m_nodes[nodeIndex];
    int endOfChildren = nodeIndex + node.m_numDescendents + 1;

    /// We have to be very careful about the m_isExpandedChanged values, which must be unaffected by this operation.
    /// Thus we use the conventional API to add/remove modifiers and setExpanded, rather than adjust them in-place.
    /// This does not cause the modifiers to appear add/removed outside the edit tree.
    std::vector<Modifier*> modifiersToAdjust;
    std::vector<FeaturePath> pathsToToggleExpansion;

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
            modPtr->adjustArrayIndex(pathToArray, startIndex, adjustment);
            modifiersAdjusted.emplace_back(std::move(modPtr));
        }
        for (auto& m : modifiersAdjusted) {
            addModifier(std::move(m));
        }
    }

    for (const auto& p : pathsToToggleExpansion) {
        setExpanded(p, c_expandedByDefault);
    }

    const unsigned int pathIndexOfStepIntoArray = pathToArray.getNumSteps();

    for (auto p : pathsToToggleExpansion) {
        assert(pathToArray.isStrictPrefixOf(p));
        babelwires::PathStep& stepIntoArray = p.getStep(pathIndexOfStepIntoArray);
        auto index = stepIntoArray.getIndex();
        assert((index >= startIndex) && "This code only applies when the index is correct.");
        stepIntoArray = index + adjustment;
        setExpanded(p, !c_expandedByDefault);
    }
}

bool babelwires::EditTree::isExpanded(const FeaturePath& featurePath) const {
    auto it = featurePath.begin();
    const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end());
    if ((nodeIndex == -1) || (it != featurePath.end())) {
        return c_expandedByDefault;
    } else {
        return m_nodes[nodeIndex].m_isExpanded;
    }
}

void babelwires::EditTree::setExpanded(const FeaturePath& featurePath, bool expanded) {
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

bool babelwires::EditTree::validateTree() const {
    std::vector<int> endOfChildrenStack;
    endOfChildrenStack.emplace_back(m_nodes.size());

    FeaturePath path;
    FeaturePath previousPath;

    for (int i = 0; i < m_nodes.size(); ++i) {
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
            assert((node.m_modifier->getPathToFeature() == path) && "Node with wrong path");
        }
    }
    for (auto endOfChildren : endOfChildrenStack) {
        assert((endOfChildren == m_nodes.size()) && "Ragged ends");
    }
    assert(path.getNumSteps() == endOfChildrenStack.size() - 1);

    return true;
}

babelwires::FeaturePath babelwires::EditTree::getPathToNode(TreeNodeIndex soughtIndex) const {
    assert((soughtIndex < m_nodes.size()) && "soughtIndex is out of range");
    FeaturePath path;
    int currentIndex = 0;

    while (currentIndex <= soughtIndex) {
        const int endOfChildren = currentIndex + m_nodes[currentIndex].m_numDescendents + 1;
        if (soughtIndex < endOfChildren) {
            path.pushStep(m_nodes[currentIndex].m_step);
            ++currentIndex;
        } else {
            currentIndex = endOfChildren;
        }
    }
    return path;
}

void babelwires::EditTree::clearChanges() {
    // Since clearing a change can make nodes unnecessary, we use removeEdit to perform the change.
    // This approach is not optimal.
    std::vector<FeaturePath> pathsWithExpansionChanges;

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

    for (const FeaturePath& path : pathsWithExpansionChanges) {
        removeEdit(path, [](TreeNode& nodeToEdit) {
            assert(nodeToEdit.m_isExpandedChanged && "Should not be considering this node.");
            nodeToEdit.m_isExpandedChanged = false;
        });
    }
    assert(validateTree());
}

void babelwires::EditTree::truncatePathAtFirstCollapsedNode(babelwires::FeaturePath& path, State state) const {
    if (m_nodes.empty()) {
        // Path is not collapsed.
        if constexpr (!c_expandedByDefault) {
            path.truncate(1);
        }
        return;
    }

    int pathIndex = 0;
    const int numPathSteps = path.getNumSteps();

    int nodeIndex = -1;
    int endOfChildren = m_nodes.size();

    while (pathIndex < numPathSteps) {
        const PathStep& current = path.getStep(pathIndex);
        int childIndex = nodeIndex + 1;
        while (childIndex < endOfChildren) {
            const TreeNode& child = m_nodes[childIndex];
            if (child.m_step == current) {
                if (((state == State::CurrentState) && !child.m_isExpanded) ||
                    ((state == State::PreviousState) && (child.m_isExpanded == child.m_isExpandedChanged))) {
                    path.truncate(pathIndex + 1);
                    return;
                }
                ++pathIndex;
                nodeIndex = childIndex;
                endOfChildren = nodeIndex + child.m_numDescendents + 1;
                break;
            } else if (current < child.m_step) {
                // The path leads outside the tree.
                if constexpr (!c_expandedByDefault) {
                    path.truncate(pathIndex + 1);
                }
                return;
            }
            childIndex += child.m_numDescendents + 1;
        }
        assert(childIndex <= endOfChildren);
        if (childIndex == endOfChildren) {
            // The path leads outside the tree.
            if constexpr (!c_expandedByDefault) {
                path.truncate(pathIndex + 1);
            }
            return;
        }
    }
}

std::vector<babelwires::FeaturePath> babelwires::EditTree::getAllExpandedPaths(const FeaturePath& featurePath) const {
    std::vector<FeaturePath> expandedPaths;

    int beginIndex = 0;
    int endIndex = m_nodes.size();
    if (featurePath.getNumSteps() != 0) {
        auto it = featurePath.begin();
        const auto [nodeIndex, _] = findNodeIndex(it, featurePath.end());
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
