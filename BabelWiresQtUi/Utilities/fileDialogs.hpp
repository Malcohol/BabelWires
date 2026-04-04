/**
 * Functions which create file dialogs for FileFormats.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <QString>

class QWidget;

namespace babelwires {

    class FileTypeEntry;

    BABELWIRESQTUI_API QString showOpenFileDialog(QWidget* parent, const FileTypeEntry& format);

    BABELWIRESQTUI_API QString showSaveFileDialog(QWidget* parent, const FileTypeEntry& format);

} // namespace babelwires
