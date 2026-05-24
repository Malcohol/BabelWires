/**
 * Functions which create file dialogs for FileFormats.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>

#include <BaseLib/Registry/fileTypeRegistry.hpp>

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
        return dialogFormats + ";;" + QObject::tr("All files (*)");
    }
} // namespace

QString babelwires::showOpenFileDialog(QWidget* parent, const FileTypeEntry& format) {
    QString dialogCaption = QObject::tr("Open ") + format.getName().c_str();
    QString dialogFormats = getFormatString(format);
    QFileDialog dialog(parent, dialogCaption, QString(), dialogFormats);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
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
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
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
