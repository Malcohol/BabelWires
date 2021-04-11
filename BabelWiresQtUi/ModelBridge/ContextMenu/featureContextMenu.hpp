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

    /// The pop-up context menu used for the rows of the FeatureModel.
    class FeatureContextMenu : public QMenu {
        Q_OBJECT
      public:
        FeatureContextMenu(FeatureModel& model, const QModelIndex& index);
        void leaveEvent(QEvent* event);

        /// Add a context menu item and wire things up correctly.
        void addFeatureContextMenuAction(FeatureContextMenuAction* action);

        FeatureModel& getModel();
        const QModelIndex& getModelIndex() const;

      private:
        FeatureModel& m_model;
        QModelIndex m_index;
    };

} // namespace babelwires
