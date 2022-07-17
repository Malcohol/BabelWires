/**
 * The OStreamLogListener writes all messages to a provided ostream.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/Log/ostreamLogListener.hpp>

#include <cassert>
#include <ctime>
#include <iomanip>

babelwires::OStreamLogListener::OStreamLogListener(std::ostream& ostream, Log& log, Features features)
    : m_ostream(ostream)
    , m_features(features) {
    m_newMessageSubscription =
        log.m_newMessage.subscribe([this](const Log::Message& message) { onNewMessage(message); });
}

void babelwires::OStreamLogListener::onNewMessage(const Log::Message& message) {
    if ((message.m_type == Log::MessageType::debugMessage) && isZero(m_features & Features::logDebugMessages)) {
        return;
    }

    if (isNonzero(m_features & Features::timestamp)) {
        const std::time_t t = Log::TimeStampClock::to_time_t(message.m_timeStamp);
        // TODO This is not thread-safe (unlikely to cause a problem, but ought to be fixed.)
        const std::tm* tm = std::localtime(&t);
        const auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(message.m_timeStamp.time_since_epoch()) % 1000;
        m_ostream << std::put_time(tm, "%F %T.");
        m_ostream << std::setfill('0') << std::setw(3) << ms.count();
        m_ostream << std::put_time(tm, " %z") << " ";
    }
    const char* prefix = nullptr;
    switch (message.m_type) {
        case Log::MessageType::infoMessage:
            prefix = "Info: ";
            break;
        case Log::MessageType::errorMessage:
            prefix = "Error: ";
            break;
        case Log::MessageType::warningMessage:
            prefix = "Warning: ";
            break;
        case Log::MessageType::debugMessage:
            prefix = "Debug: ";
            break;
    }
    assert(prefix);
    m_ostream << prefix << message.m_contents << std::endl;
}
