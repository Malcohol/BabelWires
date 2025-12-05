/**
 * A wrapper for QMessageBox.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QMessageBox>

namespace babelwires {

    /// Show a message box titled "Warning" with the given parameters.
    /// On MacOs, the title will not be shown, so the text should include that information.
    int showWarningMessageBox(QWidget* parent, const QString& text, const QString& informativeText, QMessageBox::StandardButtons buttons,
                       QMessageBox::StandardButton defaultButton);

    /// Show a message box titled "Error" with the given parameters.
    /// On MacOs, the title will not be shown, so the text should include that information.
    int showErrorMessageBox(QWidget* parent, const QString& text, const QString& informativeText, QMessageBox::StandardButtons buttons,
                       QMessageBox::StandardButton defaultButton);
    
} // namespace babelwires