
template <typename BLOCKSTREAM, typename EVENT> struct babelwires::BlockStream::Iterator {
    using value_type = EVENT;

    bool operator==(const Iterator& other) const {
        assert((m_blockStream == other.m_blockStream) &&
               "Comparison is only supported for iterators of the same stream");
        return (m_blockIndex == other.m_blockIndex) && (m_current == other.m_current);
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

    EVENT& operator*() const { return *m_current; }

    EVENT* operator->() const { return m_current; }

    Iterator& operator++() {
        if (m_current->m_numBytesToNextEvent > 0) {
            auto* nextEvent = m_current->getNextEventInBlock();
            assert((dynamic_cast<EVENT*>(nextEvent) != nullptr) && "The stream contained an event of unexpected type");
            m_current = static_cast<EVENT*>(nextEvent);
        } else {
            assert((m_current == m_blockStream->m_blocks[m_blockIndex]->m_lastEvent) &&
                   "The event is not the last in its block");
            ++m_blockIndex;
            if (m_blockIndex == m_blockStream->m_blocks.size()) {
                m_current = nullptr;
            } else {
                auto* nextEvent = m_blockStream->m_blocks[m_blockIndex]->m_firstEvent;
                assert((dynamic_cast<EVENT*>(nextEvent) != nullptr) &&
                       "The stream contained an event of unexpected type");
                m_current = static_cast<EVENT*>(nextEvent);
            }
        }
        return *this;
    }

    BLOCKSTREAM* m_blockStream;
    /// This will be m_blockStream->m_blocks.size() in the end iterator
    std::size_t m_blockIndex = 0;
    /// This will be nullptr in the end iterator
    EVENT* m_current = nullptr;
};

template <typename EVENT>
babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT> babelwires::BlockStream::begin_impl() {
    if (!m_blocks.empty()) {
        return Iterator<babelwires::BlockStream, EVENT>{this, 0, static_cast<EVENT*>(m_blocks[0]->m_firstEvent)};
    } else {
        return end_impl<EVENT>();
    }
}

template <typename EVENT>
babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>
babelwires::BlockStream::begin_impl() const {
    if (!m_blocks.empty()) {
        return Iterator<const babelwires::BlockStream, const EVENT>{this, 0,
                                                                    static_cast<EVENT*>(m_blocks[0]->m_firstEvent)};
    } else {
        return end_impl<EVENT>();
    }
}

template <typename EVENT>
babelwires::BlockStream::Iterator<babelwires::BlockStream, EVENT> babelwires::BlockStream::end_impl() {
    return Iterator<BlockStream, EVENT>{this, m_blocks.size(), nullptr};
}

template <typename EVENT>
babelwires::BlockStream::Iterator<const babelwires::BlockStream, const EVENT>
babelwires::BlockStream::end_impl() const {
    return Iterator<const BlockStream, const EVENT>{this, m_blocks.size(), nullptr};
}
