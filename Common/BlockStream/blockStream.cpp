#include "Common/BlockStream/blockStream.hpp"

babelwires::BlockStream::BlockStream(const BlockStream& other) {
    for (const auto& event : other) {
        addEvent(event);
    }
}

babelwires::BlockStream::BlockStream(BlockStream&& other) {
    m_blocks = std::move(other.m_blocks);
    m_numEvents = other.m_numEvents;
    other.m_numEvents = 0;
}

babelwires::BlockStream& babelwires::BlockStream::operator=(const BlockStream& other) {
    clear();
    for (const auto& event : other) {
        addEvent(event);
    }
    return *this;
}

babelwires::BlockStream& babelwires::BlockStream::operator=(BlockStream&& other) {
    clear();
    m_blocks = std::move(other.m_blocks);
    m_numEvents = other.m_numEvents;
    other.m_numEvents = 0;
    return *this;
}

void babelwires::BlockStream::clear() {
    m_blocks.clear();
    m_numEvents = 0;
}

std::tuple<babelwires::BlockStream::DataBlock*, void*>
babelwires::BlockStream::getAddressForNewEvent(const StreamEvent& srcEvent) {
    DataBlock* lastBlock = nullptr;
    void* addressForNewEvent = nullptr;
    bool needNewBlock = m_blocks.empty();
    do {
        void* startOfAvailableSpace = nullptr;
        std::size_t availableSpace = 0;

        if (needNewBlock) {
            lastBlock = m_blocks.emplace_back(std::make_unique<DataBlock>()).get();
            startOfAvailableSpace = lastBlock->m_data;
            availableSpace = c_blockSize;
            assert(((srcEvent.getAlignment() + srcEvent.getSize()) < c_blockSize) && "Event is too big");
        } else {
            lastBlock = m_blocks.back().get();
            babelwires::Byte* blockEnd = lastBlock->m_data + c_blockSize;
            startOfAvailableSpace =
                reinterpret_cast<babelwires::Byte*>(lastBlock->m_lastEvent) + lastBlock->m_lastEvent->getSize();
            assert((static_cast<babelwires::Byte*>(startOfAvailableSpace) <= blockEnd) &&
                   "The last event overran the end of its block");
            availableSpace = blockEnd - static_cast<babelwires::Byte*>(startOfAvailableSpace);
        }
        addressForNewEvent =
            std::align(srcEvent.getAlignment(), srcEvent.getSize(), startOfAvailableSpace, availableSpace);
        assert((!needNewBlock || addressForNewEvent) && "Couldn't add new event to new block");
        needNewBlock = true;
    } while (addressForNewEvent == nullptr);
    return {lastBlock, addressForNewEvent};
}

void babelwires::BlockStream::fixBlockAfterEventAdded(DataBlock& block, StreamEvent& newEvent) {
    if (block.m_firstEvent == nullptr) {
        block.m_firstEvent = &newEvent;
    } else {
        block.m_lastEvent->m_numBytesToNextEvent =
            reinterpret_cast<babelwires::Byte*>(&newEvent) - reinterpret_cast<babelwires::Byte*>(block.m_lastEvent);
    }
    block.m_lastEvent = &newEvent;
    ++m_numEvents;
}

babelwires::StreamEvent& babelwires::BlockStream::addEventInternal(babelwires::StreamEvent&& srcEvent) {
    auto [block, addressForNewEvent] = getAddressForNewEvent(srcEvent);
    StreamEvent& newEvent = std::move(srcEvent).cloneInto(addressForNewEvent);
    fixBlockAfterEventAdded(*block, newEvent);
    return newEvent;
}

babelwires::StreamEvent& babelwires::BlockStream::addEventInternal(const babelwires::StreamEvent& srcEvent) {
    auto [block, addressForNewEvent] = getAddressForNewEvent(srcEvent);
    StreamEvent& newEvent = srcEvent.cloneInto(addressForNewEvent);
    fixBlockAfterEventAdded(*block, newEvent);
    return newEvent;
}

int babelwires::BlockStream::getNumEvents() const {
    return m_numEvents;
}

babelwires::BlockStream::DataBlock::~DataBlock() {
    StreamEvent* event = m_firstEvent;
    event->~StreamEvent();
    while (event != m_lastEvent) {
        event = event->getNextEventInBlock();
        event->~StreamEvent();
    }
}

babelwires::BlockStream::iterator babelwires::BlockStream::end() {
    return end_impl<StreamEvent>();
}

babelwires::BlockStream::iterator babelwires::BlockStream::begin() {
    return begin_impl<StreamEvent>();
}

babelwires::BlockStream::const_iterator babelwires::BlockStream::end() const {
    return end_impl<StreamEvent>();
}

babelwires::BlockStream::const_iterator babelwires::BlockStream::begin() const {
    return begin_impl<StreamEvent>();
}
