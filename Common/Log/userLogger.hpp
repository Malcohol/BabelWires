/**
 * The UserLogger is an interface for submitting user log messages.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Log/log.hpp"

namespace babelwires {

    /// An interface for submitting user log messages.
    /// We separate this from the debug log, because the code should control who can issue user-visible messages
    /// but should not control who can issue debug messages.
    struct UserLogger {
        /// Returns an object that has the same kind of operator<< behaviour as an ostream.
        virtual Log::MessageBuilder logInfo() = 0;
        virtual Log::MessageBuilder logError() = 0;
        virtual Log::MessageBuilder logWarning() = 0;
    };

} // namespace babelwires
