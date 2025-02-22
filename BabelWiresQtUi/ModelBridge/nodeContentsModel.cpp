/**
 * FeatureModel is the QAbstractTableModel which represents the data in a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
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
#include <QMouseEvent>
#include <QPainter>

#include <cassert>

babelwires::NodeContentsModel::NodeContentsModel(QObject* parent, NodeId elementId, ProjectBridge& projectBridge)
    : QAbstractTableModel(parent)
    , m_projectBridge(projectBridge)
    , m_nodeId(elementId) {}

int babelwires::NodeContentsModel::getNumRows(AccessModelScope& scope) const {
    if (const Node* node = getNode(scope)) {
        return node->getContentsCache().getNumRows();
    } else {
        return 0;
    }
}

int babelwires::NodeContentsModel::rowCount(const QModelIndex& /*parent*/) const {
    AccessModelScope scope(m_projectBridge);
    return getNumRows(scope);
}

const babelwires::ContentsCacheEntry* babelwires::NodeContentsModel::getEntry(AccessModelScope& scope, int row) const {
    if (const Node* node = getNode(scope)) {
        return node->getContentsCache().getEntry(row);
    }
    return nullptr;
}

const babelwires::ContentsCacheEntry* babelwires::NodeContentsModel::getEntry(AccessModelScope& scope,
                                                                         const QModelIndex& index) const {
    return getEntry(scope, index.row());
}

const babelwires::Node* babelwires::NodeContentsModel::getNode(AccessModelScope& scope) const {
    return scope.getProject().getNode(m_nodeId);
}

int babelwires::NodeContentsModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

QVariant babelwires::NodeContentsModel::data(const QModelIndex& index, int role) const {
    AccessModelScope scope(m_projectBridge);
    const Node* node = getNode(scope);
    if (!node) {
        return QVariant();
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return QVariant();
    }

    const int column = index.column();

    const ValueTreeNode* valueTreeNode = entry->getInputThenOutput();
    assert(valueTreeNode && "No valueTreeNode for row model");
    const babelwires::UiProjectContext& context = m_projectBridge.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    switch (role) {
        case Qt::DisplayRole: {
            if (column == 0) {
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
                assert((column == 1) && "Column out of range");
                return rowModel->getValueDisplayData();
            }
        }
        case Qt::ToolTipRole: {
            return rowModel->getTooltip();
        }
        case Qt::BackgroundRole: {
            switch (rowModel->getBackgroundStyle(column ? RowModel::ColumnType::Value : RowModel::ColumnType::Key)) {
                case RowModel::BackgroundStyle::normal:
                    return QBrush(QColor(225, 225, 225));
                case RowModel::BackgroundStyle::editable:
                    return QBrush(QColor(255, 255, 255));
                case RowModel::BackgroundStyle::failed:
                    return QBrush(QColor(225, 30, 30));
                case RowModel::BackgroundStyle::failedHidden:
                    return QBrush(QColor(225, 150, 150));
            }
        }
        case Qt::FontRole: {
            if (rowModel->isFeatureModified()) {
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

    AccessModelScope scope(m_projectBridge);
    if (const Node* node = getNode(scope)) {
        if (const babelwires::ContentsCacheEntry* entry = getEntry(scope, index)) {
            const babelwires::UiProjectContext& context = m_projectBridge.getContext();
            RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

            if (rowModel->isItemEditable()) {
                flags = flags | Qt::ItemIsEditable;
            }
        }
    }
    return flags;
}

QMenu* babelwires::NodeContentsModel::getContextMenu(const QModelIndex& index) {
    AccessModelScope scope(m_projectBridge);
    const Node* node = getNode(scope);
    if (!node) {
        return nullptr;
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return nullptr;
    }

    const babelwires::UiProjectContext& context = m_projectBridge.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    std::vector<ContextMenuEntry> entries;
    rowModel->getContextMenuActions(entries);
    if (!entries.empty()) {
        ContextMenu* menu = new ContextMenu(*this, index);
        for (auto&& entry : entries) {
            menu->addContextMenuEntry(std::move(entry));
        }
        return menu;
    }
    return nullptr;
}

babelwires::ProjectBridge& babelwires::NodeContentsModel::getProjectBridge() {
    return m_projectBridge;
}

babelwires::NodeId babelwires::NodeContentsModel::getNodeId() const {
    return m_nodeId;
}

void babelwires::NodeContentsModel::onClicked(const QModelIndex& index) const {
    const int column = index.column();
    if (column == 0) {
        AccessModelScope scope(m_projectBridge);
        const ContentsCacheEntry* entry = getEntry(scope, index);
        if (entry->isExpandable()) {
            const NodeId elementId = getNodeId();
            const char* const actionName = entry->isExpanded() ? "Collapse compound" : "Expand compound";
            m_projectBridge.scheduleCommand(
                std::make_unique<SetExpandedCommand>(actionName, elementId, entry->getPath(), !entry->isExpanded()));
        }
    }
}
