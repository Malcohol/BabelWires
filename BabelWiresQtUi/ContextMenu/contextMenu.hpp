/**
 * The pop-up context menu used for the elements of an QAbstractItemModel.
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
#include <QAbstractItemModel>

namespace babelwires {
    /// The pop-up context menu used for the elements of an QAbstractItemModel.
    class ContextMenu : public QMenu {
        Q_OBJECT
      public:
        ContextMenu(QAbstractItemModel& model, const QModelIndex& index);
        void leaveEvent(QEvent* event);

        void addContextMenuEntry(ContextMenuEntry entry);

        QAbstractItemModel& getModel();
        const QModelIndex& getModelIndex() const;

      private:
        /// Add a group and wire things up correctly.
        void addContextMenuGroup(ContextMenuGroup* group);

        /// Add a context menu item and wire things up correctly.
        void addContextMenuAction(ContextMenuAction* action);

      private:
        QAbstractItemModel& m_model;
        QModelIndex m_index;
    };

} // namespace babelwires
