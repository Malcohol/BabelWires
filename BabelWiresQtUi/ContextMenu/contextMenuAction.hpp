/**
 * The pop-up context menu used for the rows of the NodeContentsModel.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QAction>
#include <QAbstractItemModel>
#include <QModelIndex>

namespace babelwires {

    /// Base class of context menu items in the context menu.
    class ContextMenuAction : public QAction {
        Q_OBJECT
      public:
        ContextMenuAction(const QString& text);

        /// Should be overridden to perform the operation on the model.
        virtual void actionTriggered(QAbstractItemModel& model, const QModelIndex& index) const = 0;

      public slots:
        /// Dispatches to actionTriggered, obtaining the arguments from the parent menu.
        void onTriggeredFired();
    };
} // namespace babelwires
