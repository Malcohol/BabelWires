/**
 * The EditTree arranges edits (modifiers and expand/collapse) in a tree organized by paths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

/// Iterate over a RootedPath.
struct babelwires::EditTree::RootedPathIterator {
    void operator++();
    bool operator==(const RootedPathIterator& other) const;
    bool operator!=(const RootedPathIterator& other) const;
    int distanceFrom(const RootedPathIterator& other) const;
    PathStep operator*() const;

    FeaturePath::const_iterator m_it;
    bool m_isAtRoot;
};

/// Acts like the path it wraps, but has an extra "NotAStep" at the front to indicate
/// the non-step to the root. This is a convenience structure which simplifies
/// a lot of the EditTree code, since RootedPaths correspond in a nicer way to
/// nodes of the EditTree.
struct babelwires::EditTree::RootedPath {
    RootedPath(const FeaturePath& path);
    RootedPathIterator begin() const;
    RootedPathIterator end() const;
    unsigned int getNumSteps() const;

    const FeaturePath& m_path;
};

template <typename EDIT_TREE> struct babelwires::EditTree::Iterator {
    using EDIT_TREE_PARAM = EDIT_TREE;

    Iterator(EDIT_TREE& tree, EditTree::TreeNodeIndex index, EditTree::TreeNodeIndex endIndex)
        : m_tree(tree)
        , m_index(index)
        , m_endIndex(endIndex) {}

    Iterator& operator=(const Iterator& other) {
        assert((&m_tree == &other.m_tree) && "Cannot assign iterators from different trees");
        m_index = other.m_index;
        m_endIndex = other.m_endIndex;
        return *this;
    }

    /// May return nullptr.
    typename CopyConst<EDIT_TREE, Modifier>::type* getModifier() { return m_tree.m_nodes[m_index].m_modifier.get(); }

    PathStep getStep() const { return m_tree.m_nodes[m_index].m_step; }

    Iterator childrenBegin() const { return Iterator(m_tree, m_index + 1, m_endIndex); }

    void nextSibling() { m_index = m_index + m_tree.m_nodes[m_index].m_numDescendents + 1; }

    Iterator childrenEnd() const {
        Iterator ret = *this;
        ret.nextSibling();
        return ret;
    }

    bool operator==(const Iterator& other) const { return m_index == other.m_index; }
    bool operator!=(const Iterator& other) const { return !(*this == other); }

    EDIT_TREE& m_tree;
    EditTree::TreeNodeIndex m_index;
    EditTree::TreeNodeIndex m_endIndex;
};

template <typename EDIT_TREE, typename MODIFIER_TYPE> struct babelwires::EditTree::ModifierIterator {
    using EDIT_TREE_PARAM = EDIT_TREE;

    ModifierIterator(EDIT_TREE& tree, EditTree::TreeNodeIndex index, EditTree::TreeNodeIndex endIndex)
        : m_tree(tree)
        , m_index(index)
        , m_endIndex(endIndex) {
        skipFilteredElements();
    }

    void skipFilteredElements() {
        // Skip over filtered modifiers.
        while (m_index < m_endIndex) {
            if (m_tree.m_nodes[m_index].m_modifier.get() != nullptr) {
                static_assert(std::is_same_v<ConnectionModifier, std::remove_const_t<MODIFIER_TYPE>> ||
                              std::is_same_v<Modifier, std::remove_const_t<MODIFIER_TYPE>>);
                if constexpr (std::is_same_v<ConnectionModifier, std::remove_const_t<MODIFIER_TYPE>>) {
                    if (m_tree.m_nodes[m_index].m_modifier->asConnectionModifier()) {
                        break;
                    }
                } else {
                    break;
                }
            }
            ++m_index;
        }
    }

    void operator++() {
        ++m_index;
        skipFilteredElements();
    }
    typename CopyConst<EDIT_TREE, MODIFIER_TYPE>::type* operator*() {
        auto* currentModifier = m_tree.m_nodes[m_index].m_modifier.get();
        assert(currentModifier->template as<MODIFIER_TYPE>() && "The filter isn't working");
        return static_cast<MODIFIER_TYPE*>(currentModifier);
    }

    bool operator==(const ModifierIterator& other) const { return m_index == other.m_index; }
    bool operator!=(const ModifierIterator& other) const { return !(*this == other); }

    EDIT_TREE& m_tree;
    EditTree::TreeNodeIndex m_index;
    EditTree::TreeNodeIndex m_endIndex;
};

template <typename ITERATOR> struct babelwires::EditTree::IteratorRange {
    IteratorRange(typename ITERATOR::EDIT_TREE_PARAM& tree, EditTree::TreeNodeIndex index,
                  EditTree::TreeNodeIndex endIndex)
        : m_tree(tree)
        , m_index(index)
        , m_endIndex(endIndex) {}

    ITERATOR begin() { return {m_tree, m_index, m_endIndex}; }

    ITERATOR end() { return {m_tree, m_endIndex, m_endIndex}; }

    ITERATOR begin() const { return {m_tree, m_index, m_endIndex}; }

    ITERATOR end() const { return {m_tree, m_endIndex, m_endIndex}; }

    typename ITERATOR::EDIT_TREE_PARAM& m_tree;
    const EditTree::TreeNodeIndex m_index;
    const EditTree::TreeNodeIndex m_endIndex;
};

template <typename MODIFIER_TYPE>
babelwires::EditTree::IteratorRange<babelwires::EditTree::ModifierIterator<babelwires::EditTree, MODIFIER_TYPE>>
babelwires::EditTree::modifierRange() {
    return {*this, 0, static_cast<TreeNodeIndex>(m_nodes.size())};
}

template <typename MODIFIER_TYPE>
babelwires::EditTree::IteratorRange<babelwires::EditTree::ModifierIterator<const babelwires::EditTree, MODIFIER_TYPE>>
babelwires::EditTree::modifierRange() const {
    return {*this, 0, static_cast<TreeNodeIndex>(m_nodes.size())};
}

template <typename MODIFIER_TYPE>
babelwires::EditTree::IteratorRange<babelwires::EditTree::ModifierIterator<babelwires::EditTree, MODIFIER_TYPE>>
babelwires::EditTree::modifierRange(const FeaturePath& path) {
    RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();

    const auto [beginIndex, _] = findNodeIndex(it, end);
    if (it == featurePath.end()) {
        const TreeNodeIndex endIndex = beginIndex + m_nodes[beginIndex].m_numDescendents + 1;
        return {*this, static_cast<TreeNodeIndex>(beginIndex), endIndex};
    } else {
        return {*this, 0, 0};
    }
}

template <typename MODIFIER_TYPE>
babelwires::EditTree::IteratorRange<babelwires::EditTree::ModifierIterator<const babelwires::EditTree, MODIFIER_TYPE>>
babelwires::EditTree::modifierRange(const FeaturePath& path) const {
    RootedPath featurePath(path);
    auto it = featurePath.begin();
    const auto end = featurePath.end();

    const auto [beginIndex, _] = findNodeIndex(it, end);
    if (it == featurePath.end()) {
        const TreeNodeIndex endIndex = beginIndex + m_nodes[beginIndex].m_numDescendents + 1;
        return {*this, static_cast<TreeNodeIndex>(beginIndex), endIndex};
    } else {
        return {*this, 0, 0};
    }
}
