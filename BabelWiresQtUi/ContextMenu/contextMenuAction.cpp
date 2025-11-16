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
    : QAction(text) {
}

void babelwires::ContextMenuAction::onTriggeredFired() {
    QWidget *const parentWidget = qobject_cast<QWidget*>(parent());
    assert(dynamic_cast<ContextMenu*>(parentWidget) && "ContextMenuAction has unexpected parent widget");
    ContextMenu *const menu = static_cast<ContextMenu*>(parentWidget);
    actionTriggered(menu->getModel(), menu->getModelIndex());
    menu->close();
}

void babelwires::ContextMenuGroup::addContextMenuAction(std::unique_ptr<ContextMenuAction> action) {
    m_actions.emplace_back(std::move(action));
}
