/**
 * The UserLogger is an interface for submitting user-visible log messages.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Log/log.hpp>

namespace babelwires {
    /// A restricted interface for submitting user-visible advisory (i.e. non-error) messages to the log.
    /// This is intended for scenarios where errors should strictly be returned in a Result object.
    /// 
    /// We separate this from the debug log, because the code should control who can issue user-visible messages
    /// but should not control who can issue debug messages.
    struct BASELIB_API UserAdvisoryLogger {
        virtual Log::MessageBuilder logInfo() = 0;
        virtual Log::MessageBuilder logWarning() = 0;
    };

    /// An interface for submitting all types of user-visible log messages.
    /// We separate this from the debug log, because the code should control who can issue user-visible messages
    /// but should not control who can issue debug messages.
    struct BASELIB_API UserLogger : UserAdvisoryLogger {
        /// Returns an object that has the same kind of operator<< behaviour as an ostream.
        virtual Log::MessageBuilder logError() = 0;
    };

} // namespace babelwires
