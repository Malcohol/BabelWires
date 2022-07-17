/**
 * The OStreamLogListener writes all messages to a provided ostream.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Log/log.hpp>
#include <Common/Signal/signalSubscription.hpp>
#include <Common/Utilities/enumFlags.hpp>

#include <ostream>

namespace babelwires {

    /// Writes all messages to a provided ostream.
    class OStreamLogListener {
      public:
        enum class Features : unsigned int { none = 0, timestamp = 1, logDebugMessages = 2 };

        OStreamLogListener(std::ostream& ostream, Log& log, Features features = Features::none);

      private:
        void onNewMessage(const Log::Message& message);

      private:
        std::ostream& m_ostream;
        Features m_features;
        SignalSubscription m_newMessageSubscription;
    };

    DEFINE_ENUM_FLAG_OPERATORS(OStreamLogListener::Features);

} // namespace babelwires
