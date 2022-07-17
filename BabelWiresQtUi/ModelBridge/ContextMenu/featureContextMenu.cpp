/**
 * The pop-up context menu used for the rows of the FeatureModel.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <cassert>

babelwires::FeatureContextMenu::FeatureContextMenu(FeatureModel& model, const QModelIndex& index)
    : m_model(model)
    , m_index(index) {}

void babelwires::FeatureContextMenu::leaveEvent(QEvent* event) {
    close();
}

void babelwires::FeatureContextMenu::addFeatureContextMenuAction(FeatureContextMenuAction* action) {
    action->setParent(this);
    connect(action, SIGNAL(triggered()), action, SLOT(onTriggeredFired()));
    addAction(action);
}

babelwires::FeatureModel& babelwires::FeatureContextMenu::getModel() {
    return m_model;
}

const QModelIndex& babelwires::FeatureContextMenu::getModelIndex() const {
    return m_index;
}

babelwires::FeatureContextMenuAction::FeatureContextMenuAction(const QString& text)
    : QAction(text) {}

void babelwires::FeatureContextMenuAction::onTriggeredFired() {
    QWidget* parent = parentWidget();
    FeatureContextMenu* menu = dynamic_cast<FeatureContextMenu*>(parent);
    assert(menu && "FeatureContextMenuAction has unexpected parent widget");
    actionTriggered(menu->getModel(), menu->getModelIndex());
    menu->close();
}
