/**
 * An interface for submitting debug log messages, and a global function for issuing debug messages.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Log/debugLogger.hpp"

babelwires::DebugLogger* babelwires::DebugLogger::g_debugLogger;

babelwires::DebugLogger* babelwires::DebugLogger::swapGlobalDebugLogger(DebugLogger* debugLogger) {
    std::swap(debugLogger, g_debugLogger);
    return debugLogger;
}
