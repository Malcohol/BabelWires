/**
 * Convenience functions for working with features.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
namespace babelwires {
    template <typename COMPOUND_FEATURE> struct ValueTreeChildRange {
        ValueTreeChildRange(COMPOUND_FEATURE& container)
            : m_container(container) {}

        // Convenience
        struct iterator {
            iterator(COMPOUND_FEATURE& container, int index)
                : m_container(container)
                , m_index(index) {}

            void operator++() { ++m_index; }
            bool operator==(const iterator& other) { return m_index == other.m_index; }
            bool operator!=(const iterator& other) { return !(*this == other); }
            typename CopyConst<COMPOUND_FEATURE, ValueTreeNode>::type* operator*() { return m_container.getChild(m_index); }

            COMPOUND_FEATURE& m_container;
            int m_index;
        };

        iterator begin() { return iterator(m_container, 0); }

        iterator end() { return iterator(m_container, m_container.getNumChildren()); }

        COMPOUND_FEATURE& m_container;
    };

    template <typename COMPOUND_FEATURE> ValueTreeChildRange<COMPOUND_FEATURE> getChildRange(COMPOUND_FEATURE* container) {
        return ValueTreeChildRange<COMPOUND_FEATURE>(*container);
    }

    template <typename COMPOUND_FEATURE> ValueTreeChildRange<COMPOUND_FEATURE> getChildRange(COMPOUND_FEATURE& container) {
        return ValueTreeChildRange<COMPOUND_FEATURE>(container);
    }
} // namespace babelwires
