/**
 * An interface for submitting debug log messages, and a global function for issuing debug messages.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Log/log.hpp>

#include <cassert>

namespace babelwires {

    /// Logs debug messages to the global debug logger.
    babelwires::Log::MessageBuilder logDebug();

    /// An interface for submitting debug log messages.
    /// There are also static members for managing access to a global debug logger.
    class DebugLogger {
      public:
        /// Returns an object that has the same kind of operator<< behaviour as an ostream.
        virtual Log::MessageBuilder logDebug() = 0;

      public:
        /// Set a debug logger as the global debug logger.
        static DebugLogger* swapGlobalDebugLogger(DebugLogger* debugLogger);

      private:
        friend babelwires::Log::MessageBuilder logDebug();
        static babelwires::DebugLogger* g_debugLogger;
    };

} // namespace babelwires

#include <Common/Log/debugLogger_inl.hpp>
