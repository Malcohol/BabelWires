/**
 * A Widget for displaying the log.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Log/log.hpp"

#include <QDockWidget>

class QPlainTextEdit;

namespace babelwires {

    class LogWindow : public QDockWidget {
        Q_OBJECT
      public:
        LogWindow(QWidget* parent, Log& log);

      public:
        void onNewMessage(const Log::Message& message);

      private:
        QPlainTextEdit* m_textWidget;
        /// The last message which was issued, so we can avoid displaying multiple copies of the same message.
        Log::Message m_lastLogMessage;
        /// The number of times the last message has been repeated without interruption.
        int m_numRepeatsOfLastMessage;
        SignalSubscription m_newMessageSubscription;
    };

} // namespace babelwires
