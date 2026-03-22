/**
 * Some common colour handling routines.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/babelWiresQtUiExport.hpp>

#include <QColor>

namespace babelwires {
    enum class BackgroundType {
        normal,
        failure,
        partialFailure
    };
    /// Get a background colour suitable for default editors.
    BABELWIRESQTUI_API QColor getBackgroundColour(BackgroundType type); 
}