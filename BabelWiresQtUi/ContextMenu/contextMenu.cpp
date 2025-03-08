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

QAbstractItemModel& babelwires::ContextMenu::getModel() {
    return m_model;
}

const QModelIndex& babelwires::ContextMenu::getModelIndex() const {
    return m_index;
}

void babelwires::ContextMenu::addContextMenuAction(ContextMenuAction* action) {
    action->setParent(this);
    connect(action, SIGNAL(triggered()), action, SLOT(onTriggeredFired()));
    addAction(action);
}

void babelwires::ContextMenu::addContextMenuGroup(ContextMenuGroup* group) {
    addSeparator()->setText(group->m_groupName);
    QActionGroup *const qgroup = new QActionGroup(this);
    qgroup->setExclusionPolicy(group->m_exclusionPolicy);
    for (auto& action : group->m_actions) {
        qgroup->addAction(action.get());
        // The menu becomes the parent of the action.
        addContextMenuAction(action.release());
    }
    addSeparator();
}

void babelwires::ContextMenu::addContextMenuEntry(ContextMenuEntry entry) {
    struct VisitorMethods {
        const void operator()(std::unique_ptr<ContextMenuAction>& action) {
            m_menu.addContextMenuAction(action.release());
        }
        const void operator()(std::unique_ptr<ContextMenuGroup>& group) {
            m_menu.addContextMenuGroup(group.release());
        }
        ContextMenu& m_menu;
    } visitorMethods{*this};
    std::visit(visitorMethods, entry);
}
