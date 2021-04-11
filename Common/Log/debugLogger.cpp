#include "Common/Log/debugLogger.hpp"

babelwires::DebugLogger* babelwires::DebugLogger::g_debugLogger;

babelwires::DebugLogger* babelwires::DebugLogger::swapGlobalDebugLogger(DebugLogger* debugLogger) {
    std::swap(debugLogger, g_debugLogger);
    return debugLogger;
}
