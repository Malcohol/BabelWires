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
#include <QStyledItemDelegate>
#include <QTableView>

#include <optional>

namespace babelwires {

    class ProjectBridge;

    class NodeContentsView : public QTableView {
        Q_OBJECT
      public:
        NodeContentsView(NodeId elementId, ProjectBridge& projectBridge);

        /// The default size is too big when there are three or fewer rows.
        QSize sizeHint() const override;

        /// The default size is too big when there are three or fewer rows.
        QSize minimumSizeHint() const override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

      private:
        ProjectBridge& m_projectBridge;
        NodeId m_nodeId;

        struct DragState {
            QPoint m_startPos;
            NodeId m_newNodeId = INVALID_NODE_ID;
        };

        std::optional<DragState> m_dragState;
    };

}