/**
 * The pop-up context menu used for the elements of an QAbstractItemModel.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <cassert>

babelwires::ContextMenu::ContextMenu(QAbstractItemModel& model, const QModelIndex& index)
    : m_model(model)
    , m_index(index) {}

void babelwires::ContextMenu::leaveEvent(QEvent* event) {
    close();
}

void babelwires::ContextMenu::addContextMenuAction(ContextMenuAction* action) {
    action->setParent(this);
    connect(action, SIGNAL(triggered()), action, SLOT(onTriggeredFired()));
    addAction(action);
}

QAbstractItemModel& babelwires::ContextMenu::getModel() {
    return m_model;
}

const QModelIndex& babelwires::ContextMenu::getModelIndex() const {
    return m_index;
}
