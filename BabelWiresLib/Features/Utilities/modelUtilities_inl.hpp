/**
 * Convenience functions for working with features.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
namespace babelwires {
    template <typename COMPOUND_FEATURE> struct SubFeaturesRange {
        SubFeaturesRange(COMPOUND_FEATURE& container)
            : m_container(container) {}

        // Convenience
        struct iterator {
            iterator(COMPOUND_FEATURE& container, int index)
                : m_container(container)
                , m_index(index) {}

            void operator++() { ++m_index; }
            bool operator==(const iterator& other) { return m_index == other.m_index; }
            bool operator!=(const iterator& other) { return !(*this == other); }
            typename CopyConst<COMPOUND_FEATURE, Feature>::type* operator*() { return m_container.getFeature(m_index); }

            COMPOUND_FEATURE& m_container;
            int m_index;
        };

        iterator begin() { return iterator(m_container, 0); }

        iterator end() { return iterator(m_container, m_container.getNumFeatures()); }

        COMPOUND_FEATURE& m_container;
    };

    template <typename COMPOUND_FEATURE> SubFeaturesRange<COMPOUND_FEATURE> subfeatures(COMPOUND_FEATURE* container) {
        return SubFeaturesRange<COMPOUND_FEATURE>(*container);
    }

    template <typename COMPOUND_FEATURE> SubFeaturesRange<COMPOUND_FEATURE> subfeatures(COMPOUND_FEATURE& container) {
        return SubFeaturesRange<COMPOUND_FEATURE>(container);
    }
} // namespace babelwires
