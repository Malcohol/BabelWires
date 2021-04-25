/**
 * Functions which create file dialogs for FileFormats.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QString>

class QWidget;

namespace babelwires {

    class FileTypeEntry;

    QString showOpenFileDialog(QWidget* parent, const FileTypeEntry& format);

    QString showSaveFileDialog(QWidget* parent, const FileTypeEntry& format);

} // namespace babelwires
