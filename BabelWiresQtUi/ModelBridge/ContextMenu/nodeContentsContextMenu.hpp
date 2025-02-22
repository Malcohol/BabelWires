/**
 * The pop-up context menu used for the rows of the NodeContentsModel.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <QAction>
#include <QMenu>
#include <QModelIndex>

#include <variant>

namespace babelwires {

    class NodeContentsModel;

    /// Convenience base class that assumes the model is a NodeContentsModel.
    class NodeContentsContextMenuAction : public ContextMenuAction {
        Q_OBJECT
      public:
        NodeContentsContextMenuAction(const QString& text);

        void actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const override final;

        /// Should be overridden to do perform the operation on the model.
        virtual void actionTriggered(NodeContentsModel& model, const QModelIndex& index) const = 0;
    };

} // namespace babelwires
