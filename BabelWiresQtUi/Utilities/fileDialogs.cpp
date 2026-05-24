/**
 * Functions which create file dialogs for FileFormats.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>

#include <BaseLib/Registry/fileTypeRegistry.hpp>

#include <cctype>

#include <QtWidgets/QFileDialog>

#include <QStringBuilder>

namespace {
    QString getFormatString(const babelwires::FileTypeEntry& format) {
        QString dialogFormats = QString(format.getName().c_str()) + " (";
        const auto& extensions = format.getFileExtensions();
        for (int i = 0; i < extensions.size(); ++i) {
            if (i > 0) {
                dialogFormats = dialogFormats + " ";
            }
            dialogFormats = dialogFormats + "*.";
#ifdef _WIN32
            // Windows native file dialogs are case-insensitive, so we can just add the extensions as they are.
            dialogFormats = dialogFormats + extensions[i].c_str();
#else
            // For non-Windows platforms, force case-insensitivity using reg-ex
            for (unsigned char c : extensions[i]) {
                dialogFormats = dialogFormats % QChar('[') % QChar(static_cast<char>(std::tolower(c)))
                                % QChar(static_cast<char>(std::toupper(c))) % QChar(']');
            }
#endif
        }
        dialogFormats = dialogFormats + ")";
        return dialogFormats + ";;" + QObject::tr("All files (*)");
    }
} // namespace

QString babelwires::showOpenFileDialog(QWidget* parent, const FileTypeEntry& format) {
    QString dialogCaption = QObject::tr("Open ") + format.getName().c_str();
    QString dialogFormats = getFormatString(format);
    QFileDialog dialog(parent, dialogCaption, QString(), dialogFormats);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QString filePath;
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            filePath = selectedFiles.first();
        }
    }
    return filePath;
}

QString babelwires::showSaveFileDialog(QWidget* parent, const FileTypeEntry& format) {
    QString dialogCaption = QObject::tr("Save ") + format.getName().c_str();
    QString dialogFormats = getFormatString(format);
    QFileDialog dialog(parent, dialogCaption, QString(), dialogFormats);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(format.getFileExtensions()[0].c_str());
    QString filePath;
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            filePath = selectedFiles.first();
        }
    }
    return filePath;
}
