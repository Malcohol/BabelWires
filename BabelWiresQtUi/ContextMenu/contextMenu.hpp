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

        void addContextMenuEntry(ContextMenuEntry entry);

        QAbstractItemModel& getModel();
        const QModelIndex& getModelIndex() const;

      private:
        /// Add a group and wire things up correctly.
        void addContextMenuGroup(std::unique_ptr<ContextMenuGroup> group);

        /// Add a context menu item and wire things up correctly.
        void addContextMenuAction(std::unique_ptr<ContextMenuAction> action);

      private:
        QAbstractItemModel& m_model;
        QModelIndex m_index;
        /// Used to add a separator if a group is followed by a bare action.
        bool m_lastEntryIsGroup = false;
    };

} // namespace babelwires
