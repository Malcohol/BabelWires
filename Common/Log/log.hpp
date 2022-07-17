/**
 * The Log is a thread-safe log class.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Signal/signal.hpp>

#include <chrono>
#include <mutex>
#include <sstream>

namespace babelwires {

    /// A tread-safe log class.
    /// This does nothing with the messages received other than fire a signal.
    /// This class has no API for receiving log messages.
    /// See the interfaces UserLogger and DebugLogger and the class UnifiedLog below.
    /// The aim of this structure is to force user-relevant logging to be managed by normal code-paths
    /// so arbitary code is not able to issue user-visible log messages.
    /// On the other hand, debug logging is arbitarily accessible via a singleton pattern (TODO)
    class Log {
      public:
        enum class MessageType { infoMessage, errorMessage, warningMessage, debugMessage };

        using TimeStampClock = std::chrono::system_clock;
        using TimeStamp = TimeStampClock::time_point;

        /// The actual data in a log message.
        struct Message {
            Message& operator=(const Message& other);

            std::string m_contents;
            TimeStamp m_timeStamp;
            MessageType m_type;
        };

      public:
        /// An RIAA object which submits a stream constructed message atomically to the log.
        /// A non-member operator<< is provided below.
        class MessageBuilder {
          public:
            MessageBuilder(MessageBuilder&& other);
            virtual ~MessageBuilder();

          public:
            /// Forwards its argument to the ostream operator<<.
            template <typename Arg>
            friend babelwires::Log::MessageBuilder& operator<<(babelwires::Log::MessageBuilder& stream, Arg&& arg) {
                stream.m_stream << std::forward<Arg>(arg);
                return stream;
            }

            /// Forward its argument to the ostream operator<<.
            /// This overload is needed to match the primary use-case, where the stream is a temporary
            /// returned from the log... functions.
            template <typename Arg>
            friend babelwires::Log::MessageBuilder operator<<(babelwires::Log::MessageBuilder&& stream, Arg&& arg) {
                stream.m_stream << std::forward<Arg>(arg);
                return std::move(stream);
            }

          private:
            friend Log;
            MessageBuilder(Log* log, MessageType type);

          private:
            std::ostringstream m_stream;
            TimeStamp m_timeStamp;
            Log* m_log;
            MessageType m_type;
        };

      public:
        /// A signal fired when a new message is logged.
        Signal<const Message&> m_newMessage;

      protected:
        /// Add a message to the log.
        void logMessage(const Message& message);

        /// Allow subclasses to create MessageBuilders.
        MessageBuilder createMessageBuilder(MessageType type) { return MessageBuilder(this, type); }

      private:
        std::mutex m_mutex;
    };

} // namespace babelwires
