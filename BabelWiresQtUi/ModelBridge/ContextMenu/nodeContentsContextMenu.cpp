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

babelwires::NodeContentsContextMenuAction::NodeContentsContextMenuAction(const QString& text)
    : ContextMenuAction(text) {
}

void babelwires::NodeContentsContextMenuAction::actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const {
    NodeContentsModel* nodeContentsModel = qobject_cast<NodeContentsModel*>(&model);
    assert(nodeContentsModel && "Action was triggered on the wrong kind of model");
    actionTriggered(*nodeContentsModel, index);
}
