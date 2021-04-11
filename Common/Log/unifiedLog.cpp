#include "Common/Log/unifiedLog.hpp"

babelwires::Log::MessageBuilder babelwires::UnifiedLog::logInfo() {
    return createMessageBuilder(MessageType::infoMessage);
}

babelwires::Log::MessageBuilder babelwires::UnifiedLog::logError() {
    return createMessageBuilder(MessageType::errorMessage);
}

babelwires::Log::MessageBuilder babelwires::UnifiedLog::logWarning() {
    return createMessageBuilder(MessageType::warningMessage);
}

babelwires::Log::MessageBuilder babelwires::UnifiedLog::logDebug() {
    return createMessageBuilder(MessageType::debugMessage);
}
