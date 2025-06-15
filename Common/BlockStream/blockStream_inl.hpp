/**
 * A container which can carry a sequence of hetrogeneous events.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename BLOCKSTREAM, typename EVENT>
struct babelwires::BlockStream::Iterator : std::bidirectional_iterator_tag {
    using value_type = EVENT;
    using difference_type = std::ptrdiff_t;

    Iterator(BLOCKSTREAM* blockStream)
        : m_blockStream(blockStream)
        , m_blockIndex(0) {
        if (!m_blockStream->m_blocks.empty()) {
            m_current = static_cast<EVENT*>(blockStream->m_blocks[0]->m_firstEvent);
        }
    }

    Iterator(BLOCKSTREAM* blockStream, bool)
        : m_blockStream(blockStream)
        , m_blockIndex(m_blockStream->m_blocks.size()) {}

    bool operator==(const Iterator& other) const {
        assert((m_blockStream == other.m_blockStream) &&
               "Comparison is only supported for iterators of the same stream");
        return (m_blockIndex == other.m_blockIndex) && (m_current == other.m_current);
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    EVENT& operator*() const { return *m_current; }

    EVENT* operator->() const { return m_current; }

    Iterator& operator++() {
        assert((m_current != nullptr) && "operator++ called in unexpected state");
        if (m_current->m_numBytesToNextEvent > 0) {
            auto* nextEvent = m_current->getNextEventInBlock();
            assert((nextEvent->template as<EVENT>() != nullptr) && "The stream contained an event of unexpected type");
            m_current = static_cast<EVENT*>(nextEvent);
        } else {
            assert((m_current == m_blockStream->m_blocks[m_blockIndex]->m_lastEvent) &&
                   "The event is not the last in its block");
            ++m_blockIndex;
            if (m_blockIndex == m_blockStream->m_blocks.size()) {
                m_current = nullptr;
            } else {
                auto* nextEvent = m_blockStream->m_blocks[m_blockIndex]->m_firstEvent;
                assert((nextEvent->template as<EVENT>() != nullptr) &&
                       "The stream contained an event of unexpected type");
                m_current = static_cast<EVENT*>(nextEvent);
            }
        }
        return *this;
    }

    Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    Iterator& operator--() {
        if (m_current == nullptr) {
            assert((m_blockIndex == m_blockStream->m_blocks.size()) && "operator-- called in unexpected state");
            --m_blockIndex;
            m_current = static_cast<EVENT*>(m_blockStream->m_blocks[m_blockIndex]->m_lastEvent);
        } else if (m_current->m_numBytesFromPreviousEvent > 0) {
            auto* previousEvent = m_current->getPreviousEventInBlock();
            assert((previousEvent->template as<EVENT>() != nullptr) &&
                   "The stream contained an event of unexpected type");
            m_current = static_cast<EVENT*>(previousEvent);
        } else {
            assert((m_current == m_blockStream->m_blocks[m_blockIndex]->m_firstEvent) &&
                   "The event is not the first in its block");
            assert((m_blockIndex > 0) && "operator-- called in unexpected state");
            --m_blockIndex;
            auto* previousEvent = m_blockStream->m_blocks[m_blockIndex]->m_lastEvent;
            assert((previousEvent->template as<EVENT>() != nullptr) &&
                   "The stream contained an event of unexpected type");
            m_current = static_cast<EVENT*>(previousEvent);
        }
        return *this;
    }

    Iterator operator--(int) {
        Iterator tmp = *this;
        --(*this);
        return tmp;
    }

    BLOCKSTREAM* m_blockStream;
    /// This will be m_blockStream->m_blocks.size() in the end iterator.
    std::size_t m_blockIndex = 0;
    /// This will be nullptr in the end and rend iterator
    EVENT* m_current = nullptr;
};

template <typename EVENT>
babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT> babelwires::BlockStream::begin_impl() {
    return Iterator<babelwires::BlockStream, EVENT>(this);
}

template <typename EVENT>
babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>
babelwires::BlockStream::begin_impl() const {
    return Iterator<const babelwires::BlockStream, const EVENT>(this);
}

template <typename EVENT>
babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT> babelwires::BlockStream::end_impl() {
    return Iterator<BlockStream, EVENT>(this, false);
}

template <typename EVENT>
babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>
babelwires::BlockStream::end_impl() const {
    return Iterator<const BlockStream, const EVENT>(this, false);
}

template <typename EVENT>
std::reverse_iterator<babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT>>
babelwires::BlockStream::rbegin_impl() {
    return std::make_reverse_iterator(begin_impl<EVENT>());
}

template <typename EVENT>
std::reverse_iterator<babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>>
babelwires::BlockStream::rbegin_impl() const {
    return std::make_reverse_iterator(begin_impl<EVENT>());
}

template <typename EVENT>
std::reverse_iterator<babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT>>
babelwires::BlockStream::rend_impl() {
    return std::make_reverse_iterator(end_impl<EVENT>());
}

template <typename EVENT>
std::reverse_iterator<babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>>
babelwires::BlockStream::rend_impl() const {
    return std::make_reverse_iterator(end_impl<EVENT>());
}
