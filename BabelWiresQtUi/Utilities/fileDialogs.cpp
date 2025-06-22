/**
 * Functions which create file dialogs for FileFormats.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>

#include <Common/Registry/fileTypeRegistry.hpp>

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
            // Force case-insensitivity using reg-ex
            for (auto c : extensions[i]) {
                dialogFormats = dialogFormats % QChar('[') % QChar(std::tolower(c)) % QChar(std::toupper(c)) % QChar(']');
            }
        }
        dialogFormats = dialogFormats + ")";
        return dialogFormats;
    }
} // namespace

QString babelwires::showOpenFileDialog(QWidget* parent, const FileTypeEntry& format) {
    QString dialogCaption = QObject::tr("Open ") + format.getName().c_str();
    QString dialogFormats = getFormatString(format);
    // Currently case sensitive for me. Could use QFileDialog::DontUseNativeDialog to get around this.
    return QFileDialog::getOpenFileName(parent, dialogCaption, QString(), dialogFormats);
}

QString babelwires::showSaveFileDialog(QWidget* parent, const FileTypeEntry& format) {
    QString dialogCaption = QObject::tr("Save ") + format.getName().c_str();
    QString dialogFormats = getFormatString(format);
    QFileDialog dialog(parent, dialogCaption, QString(), dialogFormats);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(format.getFileExtensions()[0].c_str());
    dialog.exec();
    QString filePath;
    QStringList selectedFiles = dialog.selectedFiles();
    if (!selectedFiles.isEmpty()) {
        filePath = selectedFiles.first();
    }
    return filePath;
}
