/**
 * The pop-up context menu used for the rows of the FeatureModel.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <QAction>
#include <QMenu>
#include <QModelIndex>

namespace babelwires {

    class FeatureModel;

    /// Base class of context menu items in the feature context menu.
    class FeatureContextMenuAction : public QAction {
        Q_OBJECT
      public:
        FeatureContextMenuAction(const QString& text);

        /// Should be overridden to do perform the operation on the model.
        virtual void actionTriggered(FeatureModel& m_model, const QModelIndex& index) const = 0;

      public slots:
        /// Dispatches to actionTriggered, obtaining the arguments from the parent menu.
        void onTriggeredFired();
    };

    /// Holds a group of actions, which will be added to the menu in a QActionGroup.
    /// By default, the actions will be exclusive.
    class FeatureContextMenuGroup {
      public:
        FeatureContextMenuGroup(QString name) : m_groupName(name) {}

        void addFeatureContextMenuAction(std::unique_ptr<FeatureContextMenuAction> action);

        QString m_groupName;
        std::vector<std::unique_ptr<FeatureContextMenuAction>> m_actions;
    };

    using FeatureContextMenuEntry = std::variant<std::unique_ptr<FeatureContextMenuAction>, std::unique_ptr<FeatureContextMenuGroup>>;

    /// The pop-up context menu used for the rows of the FeatureModel.
    class FeatureContextMenu : public QMenu {
        Q_OBJECT
      public:
        FeatureContextMenu(FeatureModel& model, const QModelIndex& index);
        void leaveEvent(QEvent* event);

        void addFeatureContextMenuEntry(FeatureContextMenuEntry entry);

        FeatureModel& getModel();
        const QModelIndex& getModelIndex() const;

      private:
        /// Add a context menu item and wire things up correctly.
        void addFeatureContextMenuAction(FeatureContextMenuAction* action);

        /// Add a group and wire things up correctly.
        void addFeatureContextMenuGroup(FeatureContextMenuGroup* group);

      private:
        FeatureModel& m_model;
        QModelIndex m_index;
    };

} // namespace babelwires
