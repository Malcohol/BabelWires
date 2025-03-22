/**
 * Some useful Qt functionality for working with BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/Utilities/qtUtils.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>

// If the popup is somewhere inside the graphicsview, then mapToGlobal doesn't work correctly.
QPoint babelwires::mapToGlobalCorrect(const QWidget* widget, QPoint point) {
    if (const auto* nodeContentsView = qobject_cast<const NodeContentsView*>(widget)) {
        return nodeContentsView->mapToGlobalCorrect(point);
    } else if (const auto* parent = qobject_cast<const QWidget*>(widget->parent())) {
        return mapToGlobalCorrect(parent, widget->mapToParent(point));
    } else {
        // The widget isn't in the graphicsview after all, so we can use regular mapToGlobal.
        return widget->mapToGlobal(point);
    }
}
