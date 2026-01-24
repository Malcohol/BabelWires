/**
 * Common functionality for objects residing in BlockStreams.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/common.hpp>

#include <cassert>

namespace babelwires {
    constexpr size_t c_maxEventSize = 256;
    constexpr size_t c_maxEventAlign = 16;
} // namespace babelwires

/// Adds boiler-plate needed by every abstract StreamEvent.
#define STREAM_EVENT_ABSTRACT(CLASS)                                                                                   \
    CLASS& cloneInto(void* memory) const& { return static_cast<CLASS&>(doCloneInto(memory)); }                         \
    CLASS& cloneInto(void* memory)&& { return static_cast<CLASS&>(std::move(*this).doCloneInto(memory)); }

/// Adds boiler-plate needed by every concrete StreamEvent.
#define STREAM_EVENT(CLASS)                                                                                            \
    STREAM_EVENT_ABSTRACT(CLASS)                                                                                       \
    std::size_t getSize() const override { return sizeof(CLASS); }                                                     \
    std::size_t getAlignment() const override { return alignof(CLASS); }                                               \
    CLASS& doCloneInto(void* memory) const& override {                                                                 \
        static_assert(sizeof(CLASS) <= babelwires::c_maxEventSize);                                                    \
        static_assert(sizeof(void*) == sizeof(std::size_t));                                                           \
        assert((reinterpret_cast<std::size_t>(memory) & (getAlignment() - 1)) == 0);                                   \
        return *new (memory) CLASS(*this);                                                                             \
    }                                                                                                                  \
    CLASS& doCloneInto(void* memory) && override {                                                                     \
        static_assert(alignof(CLASS) <= babelwires::c_maxEventAlign);                                                  \
        static_assert(sizeof(void*) == sizeof(std::size_t));                                                           \
        assert((reinterpret_cast<std::size_t>(memory) & (getAlignment() - 1)) == 0);                                   \
        return *new (memory) CLASS(std::move(*this));                                                                  \
    }

namespace babelwires {
    /// All StreamEvents are required to implement this interface.
    struct Streamable {
        // Streamable
        virtual std::size_t getSize() const = 0;
        virtual std::size_t getAlignment() const = 0;
        virtual Streamable& doCloneInto(void* memory) const& = 0;
        virtual Streamable& doCloneInto(void* memory) && = 0;
    };

    class BlockStream;

    /// The base types of events which can be placed into streams.
    /// Such events are required to be both copy and move-constructable.
    /// They should also provide implementations of the Streamable interface via the STREAM_EVENT macro.
    /// Events will be properly destroyed, so they may own other data.
    class StreamEvent : public Streamable {
      public:
        STREAM_EVENT(StreamEvent);
        DOWNCASTABLE_TYPE_HIERARCHY(StreamEvent);

        StreamEvent() = default;
        StreamEvent(const StreamEvent& other)
            : m_numBytesToNextEvent(0) {}
        StreamEvent(StreamEvent&& other)
            : m_numBytesToNextEvent(0) {}

        virtual ~StreamEvent() = default;

      private:
        friend BlockStream;

        StreamEvent* getNextEventInBlock() {
            assert(m_numBytesToNextEvent && "There is no next event in this block");
            return reinterpret_cast<StreamEvent*>(reinterpret_cast<babelwires::Byte*>(this) + m_numBytesToNextEvent);
        }

        const StreamEvent* getNextEventInBlock() const {
            assert(m_numBytesToNextEvent && "There is no next event in this block");
            return reinterpret_cast<const StreamEvent*>(reinterpret_cast<const babelwires::Byte*>(this) +
                                                        m_numBytesToNextEvent);
        }

        StreamEvent* getPreviousEventInBlock() {
            assert(m_numBytesFromPreviousEvent && "There is no previous event in this block");
            return reinterpret_cast<StreamEvent*>(reinterpret_cast<babelwires::Byte*>(this) - m_numBytesFromPreviousEvent);
        }

        const StreamEvent* getPreviousEventInBlock() const {
            assert(m_numBytesFromPreviousEvent && "There is no previous event in this block");
            return reinterpret_cast<const StreamEvent*>(reinterpret_cast<const babelwires::Byte*>(this) -
                                                        m_numBytesFromPreviousEvent);
        }

      private:
        /// The number of bytes from the address of this event to the address of the next event.
        std::uint16_t m_numBytesToNextEvent = 0;

        /// The number of bytes from the address of the preview event to the address of this event.
        std::uint16_t m_numBytesFromPreviousEvent = 0;
    };
} // namespace babelwires
