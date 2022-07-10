/**
 * The pop-up context menu used for the elements of an QAbstractItemModel.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>

#include <cassert>

babelwires::ContextMenuAction::ContextMenuAction(const QString& text)
    : QAction(text) {}

void babelwires::ContextMenuAction::onTriggeredFired() {
    QWidget *const parent = parentWidget();
    assert(dynamic_cast<ContextMenu*>(parent) && "ContextMenuAction has unexpected parent widget");
    ContextMenu *const menu = static_cast<ContextMenu*>(parent);
    actionTriggered(menu->getModel(), menu->getModelIndex());
    menu->close();
}
