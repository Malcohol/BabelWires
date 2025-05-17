/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ContextMenu/contextMenuAction.hpp>

#include <BabelWiresLib/Project/projectIds.hpp>

#include <QAbstractTableModel>
#include <QAction>
#include <QMenu>

namespace babelwires {

    class ContentsCache;
    class Node;
    class ContentsCacheEntry;
    class ProjectGraphModel;
    class AccessModelScope;
    class Path;

    /// Presents the contents of the contentsCache as a table model.
    class NodeContentsModel : public QAbstractTableModel {
        Q_OBJECT
      public:
        NodeContentsModel(QObject* parent, NodeId elementId, ProjectGraphModel& projectGraphModel);

        int rowCount(const QModelIndex& /*parent*/) const override;
        int columnCount(const QModelIndex& /*parent*/) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        /// Access the associated Node.
        const Node* getNode(const AccessModelScope& scope) const;
        /// Access an entry in the ContentsCache.
        const ContentsCacheEntry* getEntry(const AccessModelScope& scope, const QModelIndex& index) const;
        const ContentsCacheEntry* getEntry(const AccessModelScope& scope, int row) const;
        /// Access the number of rows.
        int getNumRows(const AccessModelScope& scope) const;

        void getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut, const QModelIndex& index);

        ProjectGraphModel& getProjectGraphModel();

        NodeId getNodeId() const;

      public slots:
        void onClicked(const QModelIndex& index) const;

      signals:
        void valuesMayHaveChanged() const;

      private:
        ProjectGraphModel& m_projectGraphModel;
        NodeId m_nodeId;
    };

} // namespace babelwires