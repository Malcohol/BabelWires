/**
 * The UnifiedLog is a Log which supports both user and debug logging.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Log/debugLogger.hpp>
#include <BaseLib/Log/log.hpp>
#include <BaseLib/Log/userLogger.hpp>

namespace babelwires {

    /// A Log which supports both user and debug logging.
    class UnifiedLog : public Log, public UserLogger, public DebugLogger {
      public:
        virtual MessageBuilder logInfo() override;
        virtual MessageBuilder logError() override;
        virtual MessageBuilder logWarning() override;
        virtual MessageBuilder logDebug() override;
    };

} // namespace babelwires
