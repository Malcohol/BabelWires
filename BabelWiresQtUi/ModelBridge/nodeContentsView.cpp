/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Project/Commands/addNodeForInputTreeValueCommand.hpp>
#include <BabelWiresLib/Project/Commands/addNodeForOutputTreeValueCommand.hpp>
#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <QtWidgets/QHeaderView>

#include <QGraphicsProxyWidget>
#include <QGraphicsView>

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

const QtNodes::NodeGraphicsObject& babelwires::NodeContentsView::getNodeGraphicsObject() const {
    const QGraphicsProxyWidget* const proxy = graphicsProxyWidget();
    assert(proxy);
    const QtNodes::NodeGraphicsObject* graphicsObject = qgraphicsitem_cast<QtNodes::NodeGraphicsObject*>(proxy->parentItem());
    assert(graphicsObject && "NodeContentsView not associated with a QtNodes::NodeGraphicsObject in the expected way.");
    return *graphicsObject;    
}

std::tuple<int, int> babelwires::NodeContentsView::getLeftRightEdgeFromWidgetLeft() const {
    const QtNodes::NodeGraphicsObject& graphicsObject = getNodeGraphicsObject();
    const QGraphicsView* const graphicsView = m_projectBridge.getFlowGraphWidget();

    const QPoint widgetTopLeftGlobal = mapToGlobalCorrect(QPoint(0,0));

    const auto graphicsObjectToGlobal = [&graphicsObject, &graphicsView](QPointF p) {
        const QPointF inSceneCoord = graphicsObject.mapToScene(p);
        const QPoint inScene = graphicsView->mapFromScene(inSceneCoord);
        const QPoint global = graphicsView->mapToGlobal(inScene);
        return global;
    };

    const QtNodes::NodeGeometry& nodeGeometry = graphicsObject.node().nodeGeometry();

    const QPointF graphicsObjectTopLeftGlobal = graphicsObjectToGlobal(QPointF(0,0));
    const QPointF graphicsObjectTopRightGlobal = graphicsObjectToGlobal(QPointF(nodeGeometry.width(), 0));

    const int leftEdgeWidgetPos = graphicsObjectTopLeftGlobal.x() - widgetTopLeftGlobal.x();
    const int rightEdgeWidgetPos = graphicsObjectTopRightGlobal.x() - widgetTopLeftGlobal.x(); 
    return { leftEdgeWidgetPos, rightEdgeWidgetPos };
}

void babelwires::NodeContentsView::mousePressEvent(QMouseEvent* event) {
    QTableView::mousePressEvent(event);
    // Left mouse or Left mouse + shift.
    if ((event->buttons() == Qt::LeftButton) && ((event->modifiers() & ~Qt::KeyboardModifier::ShiftModifier) == 0)) {
        const auto [left, right] = getLeftRightEdgeFromWidgetLeft();
        m_dragState = DragState{event->pos(), event->modifiers(), left, right};
    } else {
        m_dragState.reset();
    }
}

void babelwires::NodeContentsView::mouseMoveEvent(QMouseEvent* event) {
    QTableView::mouseMoveEvent(event);
    if (!m_dragState) {
        return;
    }
    if (m_dragState->m_modifiers != event->modifiers()) {
        m_dragState.reset();
        return;
    }
    if (m_dragState->m_newNodeId == INVALID_NODE_ID) {
        if (columnAt(m_dragState->m_startPos.rx()) != 0) {
            // Dragging only applies to the first column (to allow for other kind of drag behaviour on values).
            return;
        }
        const int row = rowAt(m_dragState->m_startPos.ry());
        if (event->pos().rx() < m_dragState->m_leftEdgeWidgetPos) {
            Path path;
            {
                AccessModelScope scope(m_projectBridge);
                const Node* const node = scope.getProject().getNode(m_nodeId);
                if (!node) {
                    return;
                }

                const ContentsCacheEntry* const cacheEntry = node->getContentsCache().getEntry(row);
                const ValueTreeNode* const input = cacheEntry->getInput();
                if (!input) {
                    // TODO Log a warning (outside scope)
                    return;
                }
                path = cacheEntry->getPath();
            }
            UiPosition positionForNewNode = getFlowScenePositionFromLocalPosition(event->pos());
            auto relationship = m_dragState->m_modifiers.testFlag(Qt::KeyboardModifier::ShiftModifier) ? AddNodeForInputTreeValueCommand::RelationshipToOldNode::Copy :  AddNodeForInputTreeValueCommand::RelationshipToOldNode::Source;
            auto command = std::make_unique<AddNodeForInputTreeValueCommand>(
                "Drag input left to make node", m_nodeId, std::move(path), positionForNewNode, relationship);
            const AddNodeForInputTreeValueCommand* const commandRawPtr = command.get();
            // Synchronous because the command has the NodeId only after it runs.
            // TODO: Would it be better to reserve the ID here, outside the project?
            if (m_projectBridge.executeCommandSynchronously(std::move(command))) {
                m_dragState->m_newNodeId = commandRawPtr->getNodeId();
            }
        } else if (event->pos().rx() > m_dragState->m_rightEdgeWidgetPos) {
            Path path;
            {
                AccessModelScope scope(m_projectBridge);
                const Node* const node = scope.getProject().getNode(m_nodeId);
                if (!node) {
                    return;
                }

                const ContentsCacheEntry* const cacheEntry = node->getContentsCache().getEntry(row);
                const ValueTreeNode* const output = cacheEntry->getOutput();
                if (!output) {
                    // TODO Log a warning (outside scope)
                    return;
                }
                path = cacheEntry->getPath();
            }
            UiPosition positionForNewNode = getFlowScenePositionFromLocalPosition(event->pos());
            auto relationship = m_dragState->m_modifiers.testFlag(Qt::KeyboardModifier::ShiftModifier) ? AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::Sibling : AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::NewParent;
            auto command = std::make_unique<AddNodeForOutputTreeValueCommand>(
                "Drag input right to make node", m_nodeId, std::move(path), positionForNewNode, relationship);
            const AddNodeForOutputTreeValueCommand* const commandRawPtr = command.get();
            // Synchronous because the command has the NodeId only after it runs.
            // TODO: Would it be better to reserve the ID here, outside the project?
            if (m_projectBridge.executeCommandSynchronously(std::move(command))) {
                m_dragState->m_newNodeId = commandRawPtr->getNodeId();
            }
        }
    } else {
        // Move node
        m_projectBridge.scheduleCommand(std::make_unique<MoveNodeCommand>(
            "Drag new node", m_dragState->m_newNodeId, getFlowScenePositionFromLocalPosition(event->pos())));
    }
}

void babelwires::NodeContentsView::mouseReleaseEvent(QMouseEvent* event) {
    QTableView::mouseReleaseEvent(event);
    m_dragState.reset();
}

QPointF babelwires::NodeContentsView::mapToScene(QPoint localPos) const {
    const QGraphicsProxyWidget* const proxy = graphicsProxyWidget();
    assert(proxy);
    return proxy->mapToScene(localPos);
}

QPoint babelwires::NodeContentsView::mapToGlobalCorrect(QPoint localPos) const {
    const QPointF posInScene = mapToScene(localPos);
    const QGraphicsView* const graphicsView = m_projectBridge.getFlowGraphWidget();
    const QPoint posInView = graphicsView->mapFromScene(posInScene);
    return graphicsView->mapToGlobal(posInView);
}


babelwires::UiPosition babelwires::NodeContentsView::getFlowScenePositionFromLocalPosition(QPoint localPos) {
    const QPointF posInScene = mapToScene(localPos);
    UiPosition flowScenePosition;
    flowScenePosition.m_x = static_cast<UiCoord>(posInScene.x());
    flowScenePosition.m_y = static_cast<UiCoord>(posInScene.y());
    return flowScenePosition;
}
