/**
 * A wrapper for QMessageBox.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Dialogs/messageBox.hpp>

int babelwires::showWarningMessageBox(const QString& text, const QString& informativeText, QMessageBox::StandardButtons buttons,
                               QMessageBox::StandardButton defaultButton) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Warning"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(buttons);
    msgBox.setDefaultButton(defaultButton);
    return msgBox.exec();
}

int babelwires::showErrorMessageBox(const QString& text, const QString& informativeText, QMessageBox::StandardButtons buttons,
                             QMessageBox::StandardButton defaultButton) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Error"));
    // The icon could also be Critical, but that sounds too severe for what is usually a local user operation failing.
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(buttons);
    msgBox.setDefaultButton(defaultButton);
    return msgBox.exec();
}
