/**
 * A container which can carry a sequence of hetrogeneous events.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/BlockStream/streamEvent.hpp>

#include <memory>
#include <vector>

namespace babelwires {
    /// A container which can carry a sequence of hetrogeneous events.
    /// Since streams will often contain objects of more than one type, the calling code should use StreamEvent::asA to
    /// inspect events. The memory is laid out in multiple blocks for reasonably efficient traversal.
    class BlockStream {
      public:
        BlockStream() = default;
        BlockStream(const BlockStream& other);
        BlockStream(BlockStream&& other);
        BlockStream& operator=(const BlockStream& other);
        BlockStream& operator=(BlockStream&& other);

        /// Add an event by moving or copying it into the track.
        template <typename EVENT> EVENT& addEvent(EVENT&& srcEvent) {
            StreamEvent& eventInTrack = addEventInternal(std::forward<EVENT>(srcEvent));
            assert(eventInTrack.template tryAs<typename std::remove_reference<EVENT>::type>() != nullptr);
            return static_cast<EVENT&>(eventInTrack);
        };

        /// Get the number of events in the track.
        int getNumEvents() const;

        /// Discard all the events.
        void clear();

      public:
        // Iteration.

        /// The first argument is just for const-ness. The second argument can be used when the stream
        /// is known to contain events of certain StreamEvent subtypes only.
        template <typename BLOCKSTREAM, typename EVENT> struct Iterator;

        using iterator = Iterator<BlockStream, StreamEvent>;
        iterator begin();
        iterator end();

        using const_iterator = Iterator<const BlockStream, const StreamEvent>;
        const_iterator begin() const;
        const_iterator end() const;

        std::reverse_iterator<iterator> rbegin();
        std::reverse_iterator<iterator> rend();
        std::reverse_iterator<const_iterator> rbegin() const;
        std::reverse_iterator<const_iterator> rend() const;

        // Iteration when the stream is known to contain events of certain StreamEvent subtypes only.
        template <typename EVENT> Iterator<BlockStream, EVENT> begin_impl();
        template <typename EVENT> Iterator<const BlockStream, const EVENT> begin_impl() const;
        template <typename EVENT> Iterator<BlockStream, EVENT> end_impl();
        template <typename EVENT> Iterator<const BlockStream, const EVENT> end_impl() const;

        template <typename EVENT> std::reverse_iterator<Iterator<BlockStream, EVENT>> rbegin_impl();
        template <typename EVENT> std::reverse_iterator<Iterator<const BlockStream, const EVENT>> rbegin_impl() const;
        template <typename EVENT> std::reverse_iterator<Iterator<BlockStream, EVENT>> rend_impl();
        template <typename EVENT> std::reverse_iterator<Iterator<const BlockStream, const EVENT>> rend_impl() const;

      public:
        // Public for testing.

        static constexpr size_t c_blockSize = 4 * 1024;

      private:
        /// Although tracks are intended to carry events of more than one type, we store the data in blocks
        /// to reduce the dispersal of memory.
        /// A data block always contains at least one event.
        struct DataBlock {
            /// As a compromise between efficiency and convenience, events are allowed to own resources.
            /// Therefore they may need to be cleaned up.
            /// (The alternative is to require events to carry all their data within their allocated space.
            /// Note that this is possible with the current design, but not required.)
            ~DataBlock();
            StreamEvent* m_firstEvent = nullptr;
            StreamEvent* m_lastEvent = nullptr;
            babelwires::Byte m_data[c_blockSize];
        };

      private:
        /// Get a block and address where a new event can be put.
        std::tuple<DataBlock*, void*> getAddressForNewEvent(const StreamEvent& srcEvent);

        /// Fix up the block after the event was added.
        void fixBlockAfterEventAdded(DataBlock& block, StreamEvent& newEvent);

        /// Move the srcEvent into the track.
        StreamEvent& addEventInternal(StreamEvent&& srcEvent);

        /// Copy the srcEvent into the track.
        StreamEvent& addEventInternal(const StreamEvent& srcEvent);

      private:
        template <typename BLOCKSTREAM, typename EVENT> friend struct Iterator;

        std::vector<std::unique_ptr<DataBlock>> m_blocks;

        /// The number of events in the whole stream.
        int m_numEvents = 0;
    };
} // namespace babelwires

#include <Common/BlockStream/blockStream_inl.hpp>