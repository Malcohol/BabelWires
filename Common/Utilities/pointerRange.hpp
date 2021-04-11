#pragma once

#include <utility>

namespace babelwires {

    template <typename CONTAINER_ITERATOR> class PointerIterator {
      public:
        PointerIterator(CONTAINER_ITERATOR iterator)
            : m_iterator(std::move(iterator)) {}

        void operator++() { ++m_iterator; }
        bool operator==(const PointerIterator& other) const { return m_iterator == other.m_iterator; }
        bool operator!=(const PointerIterator& other) const { return !(*this == other); }
        auto operator*() const { return m_iterator->get(); }
        auto operator->() const { return m_iterator->get(); }

      private:
        CONTAINER_ITERATOR m_iterator;
    };

    /// Expose a container of smart pointers as if it was a range of raw pointers.
    template <typename CONTAINER> class PointerRange {
      public:
        PointerRange(CONTAINER& container)
            : m_container(container) {}

        auto begin() { return PointerIterator(m_container.begin()); }

        auto end() { return PointerIterator(m_container.end()); }

      private:
        CONTAINER& m_container;
    };

    template <typename CONTAINER> class PointerRange<const CONTAINER> {
      public:
        PointerRange(const CONTAINER& container)
            : m_container(container) {}

        auto begin() const { return PointerIterator(m_container.cbegin()); }

        auto end() const { return PointerIterator(m_container.cend()); }

      private:
        const CONTAINER& m_container;
    };

} // namespace babelwires
