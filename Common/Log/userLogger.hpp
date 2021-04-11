#pragma once

#include "Common/Log/log.hpp"

namespace babelwires {

    /// An interface for submitting user log messages.
    struct UserLogger {
        /// Returns an object that has the same kind of operator<< behaviour as an ostream.
        virtual Log::MessageBuilder logInfo() = 0;
        virtual Log::MessageBuilder logError() = 0;
        virtual Log::MessageBuilder logWarning() = 0;
    };

} // namespace babelwires
