/**
 * NodeContentsModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/Utilities/colours.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>
#include <BabelWiresQtUi/ModelBridge/rowModelDelegate.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>
#include <BabelWiresLib/ValueNames/valueNames.hpp>

#include <QApplication>
#include <QBrush>
#include <QComboBox>
#include <QFont>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include <cassert>

babelwires::NodeContentsModel::NodeContentsModel(QObject* parent, NodeId elementId, ProjectGraphModel& projectGraphModel)
    : QAbstractTableModel(parent)
    , m_projectGraphModel(projectGraphModel)
    , m_nodeId(elementId) {}

int babelwires::NodeContentsModel::getNumRows(const AccessModelScope& scope) const {
    if (const Node* node = getNode(scope)) {
        return node->getContentsCache().getNumRows();
    } else {
        return 0;
    }
}

int babelwires::NodeContentsModel::rowCount(const QModelIndex& /*parent*/) const {
    AccessModelScope scope(m_projectGraphModel);
    return getNumRows(scope);
}

const babelwires::ContentsCacheEntry* babelwires::NodeContentsModel::getEntry(const AccessModelScope& scope, int row) const {
    if (const Node* node = getNode(scope)) {
        return node->getContentsCache().getEntry(row);
    }
    return nullptr;
}

const babelwires::ContentsCacheEntry* babelwires::NodeContentsModel::getEntry(const AccessModelScope& scope,
                                                                         const QModelIndex& index) const {
    return getEntry(scope, index.row());
}

const babelwires::Node* babelwires::NodeContentsModel::getNode(const AccessModelScope& scope) const {
    return scope.getProject().getNode(m_nodeId);
}

int babelwires::NodeContentsModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

QVariant babelwires::NodeContentsModel::data(const QModelIndex& index, int role) const {
    AccessModelScope scope(m_projectGraphModel);
    const Node* node = getNode(scope);
    if (!node) {
        return QVariant();
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return QVariant();
    }

    const RowModel::ColumnType columnType = index.column() ? RowModel::ColumnType::Value : RowModel::ColumnType::Key;

    const ValueTreeNode* valueTreeNode = entry->getInputThenOutput();
    assert(valueTreeNode && "No valueTreeNode for row model");
    const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    switch (role) {
        case Qt::DisplayRole: {
            if (columnType == RowModel::ColumnType::Key) {
                if (entry->isExpandable()) {
                    const char* triangle;
                    if (entry->isExpanded()) {
                        triangle = entry->hasSubmodifiers() ? "\u25bc" : "\u25bd";
                    } else {
                        triangle = entry->hasSubmodifiers() ? "\u25b6" : "\u25b7";
                    }
                    return QString(entry->getIndent() * 2 - 1, ' ') + triangle + entry->getLabel().c_str();
                } else {
                    return QString(entry->getIndent() * 2, ' ') + entry->getLabel().c_str();
                }
            } else {
                return rowModel->getValueDisplayData();
            }
        }
        case Qt::ToolTipRole: {
            return rowModel->getTooltip(columnType);
        }
        case Qt::BackgroundRole: {
            const babelwires::RowModel::BackgroundStyle backgroundStyle = rowModel->getBackgroundStyle(columnType);
            BackgroundType backgroundType = BackgroundType::normal;
            switch(backgroundStyle) {
                case RowModel::BackgroundStyle::failed:
                    backgroundType = BackgroundType::failure;
                    break;
                case RowModel::BackgroundStyle::failedHidden:
                    backgroundType = BackgroundType::partialFailure;
                    break;
                case RowModel::BackgroundStyle::normal:
                case RowModel::BackgroundStyle::editable:
                    break;
            }
            return getBackgroundColour(backgroundType);
        }
        case Qt::FontRole: {
            if (rowModel->isModified()) {
                QFont font;
                font.setBold(true);
                return font;
            }
            break;
        }
        default:
            break;
    }
    return QVariant();
}

Qt::ItemFlags babelwires::NodeContentsModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    AccessModelScope scope(m_projectGraphModel);
    if (const Node* node = getNode(scope)) {
        if (const babelwires::ContentsCacheEntry* entry = getEntry(scope, index)) {
            const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
            RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

            if (rowModel->isItemEditable()) {
                flags = flags | Qt::ItemIsEditable;
            }
        }
    }
    return flags;
}

void babelwires::NodeContentsModel::getContextMenuActions(std::vector<ContextMenuEntry>& actionsOut, const QModelIndex& index) {
    AccessModelScope scope(m_projectGraphModel);
    const Node* node = getNode(scope);
    if (!node) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return;
    }

    const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    rowModel->getContextMenuActions(actionsOut);
}

babelwires::ProjectGraphModel& babelwires::NodeContentsModel::getProjectGraphModel() {
    return m_projectGraphModel;
}

babelwires::NodeId babelwires::NodeContentsModel::getNodeId() const {
    return m_nodeId;
}

void babelwires::NodeContentsModel::onClicked(const QModelIndex& index) const {
    const int column = index.column();
    if (column == 0) {
        AccessModelScope scope(m_projectGraphModel);
        const ContentsCacheEntry* entry = getEntry(scope, index);
        if (entry->isExpandable()) {
            const NodeId elementId = getNodeId();
            const char* const actionName = entry->isExpanded() ? "Collapse compound" : "Expand compound";
            m_projectGraphModel.scheduleCommand(
                std::make_unique<SetExpandedCommand>(actionName, elementId, entry->getPath(), !entry->isExpanded()));
        }
    }
}
