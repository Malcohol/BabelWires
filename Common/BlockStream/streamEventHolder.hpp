/**
 * A StreamEventHolder is a container for a single mutable StreamEvent.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/BlockStream/streamEvent.hpp>

namespace babelwires {
    /// A container for a single mutable StreamEvent.
    /// This is useful when copying event from one stream to another, modifying it in between.
    template <typename EVENT> class StreamEventHolder {
      public:
        StreamEventHolder() = default;

        /// Construct a StreamEventHolder by copying or moving the given event into the buffer.
        template <typename EVENT2, typename = std::enable_if_t<std::is_convertible_v<EVENT2, EVENT>>>
        StreamEventHolder(EVENT2&& srcEvent) {
            m_event = &std::forward<EVENT2>(srcEvent).cloneInto(m_buffer);
        };

        /// Replace the carried event by the given one, either copying or moving it.
        template <typename EVENT2, typename = std::enable_if_t<std::is_convertible_v<EVENT2, EVENT>>>
        StreamEventHolder& operator=(EVENT2&& srcEvent) {
            reset();
            m_event = &std::forward<EVENT2>(srcEvent).cloneInto(m_buffer);
            return *this;
        }

        /// Construct a StreamEventHolder by cloning the other's event.
        StreamEventHolder(const StreamEventHolder& other) {
            if (other.m_event) {
                *this = *other.m_event;
            }
        }

        StreamEventHolder& operator=(const StreamEventHolder& other) {
            reset();
            if (other.m_event) {
                *this = *other.m_event;
            }
            return *this;
        }

        StreamEventHolder& operator=(StreamEventHolder&& other) {
            reset();
            if (other.m_event) {
                *this = std::move(*other.m_event);
            }
            return *this;
        }

        /// Construct a StreamEventHolder by moving the other's event.
        StreamEventHolder(StreamEventHolder&& other) {
            if (other.m_event) {
                *this = *std::move(other.m_event);
            }
        }

        ~StreamEventHolder() { reset(); }

        EVENT& operator*() {
            assert(m_event);
            return *m_event;
        }

        const EVENT& operator*() const {
            assert(m_event);
            return *m_event;
        }

        EVENT* operator->() {
            assert(m_event);
            return m_event;
        }

        const EVENT* operator->() const {
            assert(m_event);
            return m_event;
        }

        bool hasEvent() const { return m_event; }

        /// This just makes the carried event available for moving.
        EVENT&& release() { return std::move(*m_event); }

        /// Destroy the contained event if there is one.
        void reset() {
            if (m_event) {
                m_event->~EVENT();
                m_event = nullptr;
            }
        }

      private:
        alignas(c_maxEventAlign) std::uint8_t m_buffer[c_maxEventSize];
        EVENT* m_event = nullptr;
    };
} // namespace babelwires
