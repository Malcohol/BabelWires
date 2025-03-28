/**
 * Some useful Qt functionality for working with BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QWidget>

namespace babelwires {
    /// Get the global point corresponding to the widget local point.
    /// This accounts for the fact that regular mapToGlobal doesn't work if a widget is inside a graphics view.
    QPoint mapToGlobalCorrect(const QWidget* widget, QPoint point);
}
