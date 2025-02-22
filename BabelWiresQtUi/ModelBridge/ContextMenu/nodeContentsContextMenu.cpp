/**
 * The pop-up context menu used for the rows of the NodeContentsModel.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenu.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <cassert>

babelwires::FeatureContextMenu::FeatureContextMenu(NodeContentsModel& model, const QModelIndex& index)
    : m_model(model)
    , m_index(index) {}

void babelwires::FeatureContextMenu::leaveEvent(QEvent* event) {
    close();
}

void babelwires::FeatureContextMenu::addFeatureContextMenuAction(NodeContentsContextMenuAction* action) {
    action->setParent(this);
    addAction(action);
}

void babelwires::FeatureContextMenu::addFeatureContextMenuGroup(FeatureContextMenuGroup* group) {
    addSeparator()->setText(group->m_groupName);
    QActionGroup *const qgroup = new QActionGroup(this);
    qgroup->setExclusionPolicy(group->m_exclusionPolicy);
    for (auto& action : group->m_actions) {
        qgroup->addAction(action.get());
        // The menu becomes the parent of the action.
        addFeatureContextMenuAction(action.release());
    }
    addSeparator();
}

void babelwires::FeatureContextMenu::addFeatureContextMenuEntry(FeatureContextMenuEntry entry) {
    struct VisitorMethods {
        const void operator()(std::unique_ptr<NodeContentsContextMenuAction>& action) {
            m_menu.addFeatureContextMenuAction(action.release());
        }
        const void operator()(std::unique_ptr<FeatureContextMenuGroup>& group) {
            m_menu.addFeatureContextMenuGroup(group.release());
        }
        FeatureContextMenu& m_menu;
    } visitorMethods{*this};
    std::visit(visitorMethods, entry);
}

babelwires::NodeContentsModel& babelwires::FeatureContextMenu::getModel() {
    return m_model;
}

const QModelIndex& babelwires::FeatureContextMenu::getModelIndex() const {
    return m_index;
}

babelwires::NodeContentsContextMenuAction::NodeContentsContextMenuAction(const QString& text)
    : QAction(text) {
    connect(this, SIGNAL(triggered()), SLOT(onTriggeredFired()));
}

void babelwires::NodeContentsContextMenuAction::onTriggeredFired() {
    QWidget* parent = parentWidget();
    FeatureContextMenu* menu = dynamic_cast<FeatureContextMenu*>(parent);
    assert(menu && "NodeContentsContextMenuAction has unexpected parent widget");
    actionTriggered(menu->getModel(), menu->getModelIndex());
    menu->close();
}

void babelwires::FeatureContextMenuGroup::addFeatureContextMenuAction(std::unique_ptr<NodeContentsContextMenuAction> action) {
    m_actions.emplace_back(std::move(action));
}
