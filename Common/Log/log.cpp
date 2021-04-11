/**
 * The Log is a thread-safe log class.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Log/log.hpp"

babelwires::Log::Message& babelwires::Log::Message::operator=(const Message& other) {
    m_contents = other.m_contents;
    m_timeStamp = other.m_timeStamp;
    m_type = other.m_type;
    return *this;
}

void babelwires::Log::logMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_newMessage.fire(message);
}

babelwires::Log::MessageBuilder::MessageBuilder(Log* log, MessageType type)
    : m_timeStamp(TimeStampClock::now())
    , m_log(log)
    , m_type(type) {}

babelwires::Log::MessageBuilder::MessageBuilder(MessageBuilder&& other)
    : m_stream(std::move(other.m_stream))
    , m_timeStamp(other.m_timeStamp)
    , m_log(other.m_log)
    , m_type(other.m_type) {
    // Stop other from doing anything on destruction.
    other.m_log = nullptr;
}

babelwires::Log::MessageBuilder::~MessageBuilder() {
    if (m_log) {
        Message message{m_stream.str(), m_timeStamp, m_type};
        m_log->logMessage(message);
    }
}
