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

#include <variant>

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

} // namespace babelwires
