/**
 * FeatureModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>

#include <QAbstractTableModel>
#include <QAction>
#include <QMenu>

namespace babelwires {

    class ContentsCache;
    class Node;
    class ContentsCacheEntry;
    class ProjectBridge;
    class AccessModelScope;
    class Path;

    /// Presents the contents of the contentsCache as a table model.
    class FeatureModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        FeatureModel(QObject* parent, NodeId elementId, ProjectBridge& projectBridge);

        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        /// Access the associated Node.
        const Node* getNode(AccessModelScope& scope) const;
        /// Access an entry in the ContentsCache.
        const ContentsCacheEntry* getEntry(AccessModelScope& scope, const QModelIndex& index) const;
        const ContentsCacheEntry* getEntry(AccessModelScope& scope, int row) const;
        /// Access the number of rows.
        int getNumRows(AccessModelScope& scope) const;

        /// May return null or a new menu.
        QMenu* getContextMenu(const QModelIndex& index);

        ProjectBridge& getProjectBridge();

        NodeId getNodeId() const;

      public slots:
        void onClicked(const QModelIndex& index) const;

      signals:
        void valuesMayHaveChanged() const;

      private:
        ProjectBridge& m_projectBridge;
        NodeId m_nodeId;
    };

} // namespace babelwires