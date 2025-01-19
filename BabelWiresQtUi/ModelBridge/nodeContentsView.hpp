/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

#include <QAbstractTableModel>
#include <QAction>
#include <QMenu>
#include <QStyledItemDelegate>
#include <QTableView>

#include <optional>

namespace QtNodes {
  class NodeGraphicsObject;
}

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

        /// Get the position in scene coordinates corresponding to a position local to this widget.
        QPointF mapToScene(QPoint localPos) const;

        /// The regular mapToGlobal of this widget does not give the correct answer.
        QPoint mapToGlobalCorrect(QPoint localPos) const;

      private:
        /// The position in scene coordinates, converted to a UiPosition.
        UiPosition getFlowScenePositionFromLocalPosition(QPoint localPos);

        /// Get the QtNode::NodeGraphicsObject which contains this widget 
        const QtNodes::NodeGraphicsObject& getNodeGraphicsObject() const;

        /// Get the horizontal distance from the left edge of the contents to the left edge of the node.
        int getLeftBorderWidth() const;

      private:
        ProjectBridge& m_projectBridge;
        NodeId m_nodeId;

        struct DragState {
            QPoint m_startPos;
            NodeId m_newNodeId = INVALID_NODE_ID;
            int m_leftBorderWidth;
        };

        std::optional<DragState> m_dragState;
    };

}