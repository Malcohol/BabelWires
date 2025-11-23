/**
 * A Widget for displaying the log.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/LogWindow/logWindow.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QStyle>
#include <QToolButton>
#include <QApplication>

#include <ctime>
#include <iomanip>
#include <sstream>

babelwires::LogWindow::LogWindow(QWidget* parent, Log& log)
    : QDockWidget(parent) {
    m_textWidget = new QPlainTextEdit(this);
    m_textWidget->setReadOnly(true);
    m_textWidget->setPlaceholderText(tr("No messages"));
    m_textWidget->setMaximumBlockCount(100);
    setWidget(m_textWidget);
    m_newMessageSubscription =
        log.m_newMessage.subscribe([this](const Log::Message& message) { onNewMessage(message); });

    QWidget* title_bar = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    {
        // For reasons I don't understand, layout->ContentMargins returns all 0s, so I query the style here.
        const int leftMargin = title_bar->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
        const int rightMargin = title_bar->style()->pixelMetric(QStyle::PM_LayoutRightMargin);
        layout->setContentsMargins(leftMargin, 0, rightMargin, 0);
    }

    title_bar->setLayout(layout);
    layout->addWidget(new QLabel(tr("Log Window")));
    {
        QIcon icon = title_bar->style()->standardIcon(QStyle::SP_LineEditClearButton, 0, title_bar);
        QToolButton* clearButton = new QToolButton(title_bar);
        clearButton->setToolTip(tr("Clear the log window"));
        clearButton->setIcon(icon);
        layout->addWidget(clearButton);
        connect(clearButton, &QToolButton::clicked, m_textWidget, &QPlainTextEdit::clear);
    }
    {
        QIcon icon = title_bar->style()->standardIcon(QStyle::SP_DockWidgetCloseButton, 0, title_bar);
        QToolButton* closeButton = new QToolButton(title_bar);
        closeButton->setToolTip(tr("Close the log window"));
        closeButton->setIcon(icon);
        layout->addWidget(closeButton);
        connect(closeButton, &QToolButton::clicked, this, &LogWindow::hide);
    }

    setTitleBarWidget(title_bar);
}

void babelwires::LogWindow::onNewMessage(const Log::Message& message) {
#ifdef NDEBUG
    // Only display debug messages in debug builds.
    if (message.m_type == Log::MessageType::debugMessage) {
        return;
    }
#endif
    if ((message.m_contents == m_lastLogMessage.m_contents) && (message.m_type == m_lastLogMessage.m_type)) {
        // This message repeats the previous one (although may have a different timestamp)
        ++m_numRepeatsOfLastMessage;
        // Erase the last line, which will be overwritten.
        QTextCursor cursor = m_textWidget->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deletePreviousChar();
    } else {
        m_lastLogMessage = message;
        m_numRepeatsOfLastMessage = 0;
    }

    QString text;
    {
        std::ostringstream os;
        if (m_numRepeatsOfLastMessage > 0) {
            const std::time_t t = Log::TimeStampClock::to_time_t(m_lastLogMessage.m_timeStamp);
            // TODO Is there a thread-safety concern here?
            const std::tm* tm = std::localtime(&t);
            os << std::put_time(tm, "%T\u2014");
        }
        {
            // Just the time for the log window.
            const std::time_t t = Log::TimeStampClock::to_time_t(message.m_timeStamp);
            // TODO Is there a thread-safety concern here?
            const std::tm* tm = std::localtime(&t);
            os << std::put_time(tm, "%T ");
        }
        if (m_numRepeatsOfLastMessage > 0) {
            os << "(x" << m_numRepeatsOfLastMessage + 1 << ") ";
        }
        text = os.str().c_str();
    }

    // This uses standard palette colors which should adapt acceptably to light/dark mode.
    // The choices should guarantee the colors are distinct.
    // Error messages are the exception, where we adopt the standard colour red.
    QColor color;
    switch (message.m_type) {
        case Log::MessageType::infoMessage:
            text += "Info: ";
            color = QApplication::palette().color(QPalette::Text);
            break;
        case Log::MessageType::errorMessage:
            text += "Error: ";
            color = Qt::red;
            break;
        case Log::MessageType::warningMessage:
            text += "Warning: ";
            color = QApplication::palette().color(QPalette::Link);
            break;
        case Log::MessageType::debugMessage:
            text += "Debug: ";
            color = QApplication::palette().color(QPalette::LinkVisited);
            break;
    }
    text += message.m_contents.c_str();

    QTextCharFormat format = m_textWidget->currentCharFormat();
    format.setForeground(color);
    m_textWidget->setCurrentCharFormat(format);
    m_textWidget->appendPlainText(text);
}
