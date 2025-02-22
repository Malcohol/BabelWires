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
    /// Holds a group of actions, which will be added to the menu in a QActionGroup.
    /// By default, the actions will be exclusive.
    class ContextMenuGroup {
      public:
        ContextMenuGroup(QString name,
                                QActionGroup::ExclusionPolicy policy = QActionGroup::ExclusionPolicy::Exclusive)
            : m_groupName(name)
            , m_exclusionPolicy(policy) {}

        void addContextMenuAction(std::unique_ptr<ContextMenuAction> action);

        QActionGroup::ExclusionPolicy m_exclusionPolicy;
        QString m_groupName;
        std::vector<std::unique_ptr<ContextMenuAction>> m_actions;
    };

    using ContextMenuEntry =
        std::variant<std::unique_ptr<ContextMenuAction>, std::unique_ptr<ContextMenuGroup>>;

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
