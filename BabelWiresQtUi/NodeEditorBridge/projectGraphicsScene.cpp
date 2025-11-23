/**
 *
 * Copyright (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphicsScene.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/processorNodeFactory.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/sourceFileNodeFactory.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/targetFileNodeFactory.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/valueNodeFactory.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

// TODO - internal
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>

#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QTreeWidget>
#include <QWidgetAction>

babelwires::ProjectGraphicsScene::ProjectGraphicsScene(ProjectGraphModel& projectGraphModel)
    : BasicGraphicsScene(projectGraphModel) {
        const auto& context = projectGraphModel.getContext();
        // TODO This maybe should be managed externally to this class.
        addNodeFactory(std::make_unique<SourceFileNodeFactory>(context));
        addNodeFactory(std::make_unique<TargetFileNodeFactory>(context));
        addNodeFactory(std::make_unique<ProcessorNodeFactory>(context));
        addNodeFactory(std::make_unique<ValueNodeFactory>(context));
    }

void babelwires::ProjectGraphicsScene::addNodeFactory(std::unique_ptr<NodeFactory> nodeFactory) {
    m_nodeFactories.emplace_back(std::move(nodeFactory));
}

void babelwires::ProjectGraphicsScene::setWidgetForDialogs(QWidget* widgetForDialogs) {
    m_widgetForDialogs = widgetForDialogs;
}

QMenu* babelwires::ProjectGraphicsScene::createSceneMenu(QPointF const scenePos) {
    QMenu* modelMenu = new QMenu();

    // Add filterbox to the context menu
    auto* txtBox = new QLineEdit(modelMenu);
    txtBox->setPlaceholderText(QStringLiteral("Filter"));
    txtBox->setClearButtonEnabled(true);

    auto* txtBoxAction = new QWidgetAction(modelMenu);
    txtBoxAction->setDefaultWidget(txtBox);

    // 1.
    modelMenu->addAction(txtBoxAction);

    // Add result treeview to the context menu
    QTreeWidget* treeView = new QTreeWidget(modelMenu);
    treeView->header()->close();

    auto* treeViewAction = new QWidgetAction(modelMenu);
    treeViewAction->setDefaultWidget(treeView);

    // 2.
    modelMenu->addAction(treeViewAction);

    for (const auto& nodeFactory : m_nodeFactories) {
        auto category = new QTreeWidgetItem(treeView);
        category->setText(0, nodeFactory->getCategoryName());
        category->setFlags(category->flags() & ~Qt::ItemIsSelectable);
        for (const auto& factoryName : nodeFactory->getFactoryNames()) {
            auto item = new QTreeWidgetItem(category);
            item->setText(0, factoryName);
        }
    }

    treeView->expandAll();

    connect(modelMenu, &QMenu::aboutToHide, [modelMenu, treeView]() {
        if (treeView->selectedItems().isEmpty()) {
            // Delete the menu here only if it is cancelled.
            modelMenu->deleteLater();
        }
    });

    connect(treeView, &QTreeWidget::itemClicked, [this, modelMenu, scenePos](QTreeWidgetItem* item, int) {
        if (!(item->flags() & (Qt::ItemIsSelectable))) {
            return;
        }
        // Delete menu when a selection is made.
        modelMenu->deleteLater();
        modelMenu->close();

        QTreeWidgetItem* parent = item->parent();
        assert(parent && "non-parents should be selectable");

        const auto factoryIt =
            std::find_if(m_nodeFactories.begin(), m_nodeFactories.end(), [parent](const auto& nodeFactory) {
                return nodeFactory->getCategoryName() == parent->text(0);
            });
        assert(factoryIt != m_nodeFactories.end());

        ProjectGraphModel* projectGraphModel = qobject_cast<ProjectGraphModel*>(&graphModel());

        (*factoryIt)->createNode(*projectGraphModel, item->text(0), scenePos);
    });

    // Setup filtering
    connect(txtBox, &QLineEdit::textChanged, [treeView](const QString& text) {
        QTreeWidgetItemIterator categoryIt(treeView, QTreeWidgetItemIterator::HasChildren);
        while (*categoryIt)
            (*categoryIt++)->setHidden(true);
        QTreeWidgetItemIterator it(treeView, QTreeWidgetItemIterator::NoChildren);
        while (*it) {
            auto modelName = (*it)->text(0);
            const bool match = (modelName.contains(text, Qt::CaseInsensitive));
            (*it)->setHidden(!match);
            if (match) {
                QTreeWidgetItem* parent = (*it)->parent();
                while (parent) {
                    parent->setHidden(false);
                    parent = parent->parent();
                }
            }
            ++it;
        }
    });

    // make sure the text box gets focus so the user doesn't have to click on it
    txtBox->setFocus();

    return modelMenu;
}

bool babelwires::ProjectGraphicsScene::isSomethingSelected() const {
    return selectedItems().size() > 0;
}

bool babelwires::ProjectGraphicsScene::areNodesSelected() const {
    for (QGraphicsItem *item : selectedItems()) {
        if (qgraphicsitem_cast<QtNodes::NodeGraphicsObject*>(item)) {
            return true;
        }
    }
    return false;
}

babelwires::ProjectGraphicsScene::SelectedObjects babelwires::ProjectGraphicsScene::getSelectedObjects() const {
    SelectedObjects selection;

    for (QGraphicsItem *item : selectedItems()) {
        if (auto nodeGraphics = qgraphicsitem_cast<QtNodes::NodeGraphicsObject*>(item)) {
            selection.m_nodeIds.emplace_back(nodeGraphics->nodeId());
        }
        if (auto connectionGraphics = qgraphicsitem_cast<QtNodes::ConnectionGraphicsObject*>(item)) {
            selection.m_connectionIds.emplace_back(connectionGraphics->connectionId());
        }
    }

    return selection;
}

void babelwires::ProjectGraphicsScene::setNodeSelected(NodeId nodeId) {
    QtNodes::NodeGraphicsObject* nodeGraphics = nodeGraphicsObject(nodeId);
    assert(nodeGraphics);
    nodeGraphics->setSelected(true);
}

void babelwires::ProjectGraphicsScene::setNodeOnTop(NodeId nodeId) {
    QtNodes::NodeGraphicsObject* nodeGraphics = nodeGraphicsObject(nodeId);
    assert(nodeGraphics);
    nodeGraphics->setZValue(std::numeric_limits<qreal>::max());
}
