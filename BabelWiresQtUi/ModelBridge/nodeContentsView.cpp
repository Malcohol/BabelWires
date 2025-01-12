/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <QMouseEvent>

#include <QtWidgets/QHeaderView>

#include <cassert>

babelwires::NodeContentsView::NodeContentsView(NodeId elementId, ProjectBridge& projectBridge)
    : m_projectBridge(projectBridge)
    , m_nodeId(elementId) {
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    verticalHeader()->setVisible(false);
    verticalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resize(sizeHint());
    setDragDropMode(DragOnly);
}

QSize babelwires::NodeContentsView::sizeHint() const {
    AccessModelScope scope(m_projectBridge);
    if (const Node* node = scope.getProject().getNode(m_nodeId)) {
        // The width is stored, and the height will stretched to fit the node by the UI.
        return QSize(node->getUiSize().m_width, 1);
    }
    return QTableView::sizeHint();
}

QSize babelwires::NodeContentsView::minimumSizeHint() const {
    return sizeHint();
}

void babelwires::NodeContentsView::mousePressEvent(QMouseEvent *event) {
    QTableView::mousePressEvent(event);
    if (event->buttons() == Qt::LeftButton) {
        m_dragState = DragState{event->pos()};
        logDebug() << event->pos().rx();
    }
}

void babelwires::NodeContentsView::mouseMoveEvent(QMouseEvent *event) {
    assert(m_dragState);
    if (m_dragState->m_newNodeId == INVALID_NODE_ID) {
        if (event->pos().rx() < 0) {
            // Create node. How do I get the ID?
        }
    } else {
        // Move node
    }
}


void babelwires::NodeContentsView::mouseReleaseEvent(QMouseEvent *event) {
    assert(m_dragState);
    logDebug() << "Released: Start point was " << m_dragState->m_startPos.rx();
    m_dragState.reset();
}
