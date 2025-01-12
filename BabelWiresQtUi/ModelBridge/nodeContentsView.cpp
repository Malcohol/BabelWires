/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addNodeForInputTreeValueCommand.hpp>
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
    QTableView::mouseMoveEvent(event);
    assert(m_dragState);
    if (m_dragState->m_newNodeId == INVALID_NODE_ID) {
        if (columnAt(m_dragState->m_startPos.rx()) != 0) {
            // Dragging only applies to the first column (to allow for other kind of drag behaviour on values).
            return;
        }
        const int row = rowAt(m_dragState->m_startPos.ry());
        if (event->pos().rx() < 0) {
            Path path;
            {
                AccessModelScope scope(m_projectBridge);
                const Node *const node = scope.getProject().getNode(m_nodeId);
                if (!node) {
                    return;
                }

                const ContentsCacheEntry* const cacheEntry = node->getContentsCache().getEntry(row);
                const ValueTreeNode* const input = cacheEntry->getInput();
                if (!input) {
                    return;
                }
                path = cacheEntry->getPath();
            }
            // TODO Use modifiers.
            auto relationship = AddNodeForInputTreeValueCommand::RelationshipToOldNode::Copy;
            auto command = std::make_unique<AddNodeForInputTreeValueCommand>("Drag input left to make node", m_nodeId, std::move(path), relationship);
            const AddNodeForInputTreeValueCommand *const commandRawPtr = command.get();
            // Synchronous because the command has the NodeId only after it runs.
            // TODO: Would it be better to reserve the ID here?
            if (m_projectBridge.executeCommandSynchronously(std::move(command))) {
                m_dragState->m_newNodeId = commandRawPtr->getNodeId();
            }
        }
    } else {
        // Move node
    }
}


void babelwires::NodeContentsView::mouseReleaseEvent(QMouseEvent *event) {
    QTableView::mouseReleaseEvent(event);
    assert(m_dragState);
    logDebug() << "Released: Start point was " << m_dragState->m_startPos.rx();
    m_dragState.reset();
}
