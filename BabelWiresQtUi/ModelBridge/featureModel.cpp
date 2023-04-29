/**
 * FeatureModel is the QAbstractTableModel which represents the data in a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>
#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
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

#include <QtWidgets/QHeaderView>

#include <cassert>

babelwires::FeatureView::FeatureView(ElementId elementId, ProjectBridge& projectBridge)
    : m_projectBridge(projectBridge)
    , m_elementId(elementId) {
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
}

QSize babelwires::FeatureView::sizeHint() const {
    AccessModelScope scope(m_projectBridge);
    if (const FeatureElement* element = scope.getProject().getFeatureElement(m_elementId)) {
        // The width is stored, and the height will stretched to fit the node by the UI.
        return QSize(element->getUiSize().m_width, 1);
    }
    return QTableView::sizeHint();
}

QSize babelwires::FeatureView::minimumSizeHint() const {
    return sizeHint();
}

babelwires::FeatureModel::FeatureModel(QObject* parent, ElementId elementId, ProjectBridge& projectBridge)
    : QAbstractTableModel(parent)
    , m_projectBridge(projectBridge)
    , m_elementId(elementId) {}

int babelwires::FeatureModel::getNumRows(AccessModelScope& scope) const {
    if (const FeatureElement* element = getFeatureElement(scope)) {
        return element->getContentsCache().getNumRows();
    } else {
        return 0;
    }
}

int babelwires::FeatureModel::rowCount(const QModelIndex& /*parent*/) const {
    AccessModelScope scope(m_projectBridge);
    return getNumRows(scope);
}

const babelwires::ContentsCacheEntry* babelwires::FeatureModel::getEntry(AccessModelScope& scope, int row) const {
    if (const FeatureElement* element = getFeatureElement(scope)) {
        return element->getContentsCache().getEntry(row);
    }
    return nullptr;
}

const babelwires::ContentsCacheEntry* babelwires::FeatureModel::getEntry(AccessModelScope& scope,
                                                                         const QModelIndex& index) const {
    return getEntry(scope, index.row());
}

const babelwires::FeatureElement* babelwires::FeatureModel::getFeatureElement(AccessModelScope& scope) const {
    return scope.getProject().getFeatureElement(m_elementId);
}

int babelwires::FeatureModel::columnCount(const QModelIndex& /*parent*/) const {
    return 2;
}

QVariant babelwires::FeatureModel::data(const QModelIndex& index, int role) const {
    AccessModelScope scope(m_projectBridge);
    const FeatureElement* element = getFeatureElement(scope);
    if (!element) {
        return QVariant();
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return QVariant();
    }

    const int column = index.column();

    const Feature* feature = entry->getInputThenOutputFeature();
    assert(feature && "No feature for row model");
    const babelwires::UiProjectContext& context = m_projectBridge.getContext();
    RowModelDispatcher rowModel(context.m_rowModelReg, context.m_valueModelReg, entry, element);

    switch (role) {
        case Qt::DisplayRole: {
            if (column == 0) {
                if (entry->isExpandable()) {
                    const char* triangle;
                    if (entry->isExpanded()) {
                        triangle = entry->hasSubmodifiers() ? u8"\u25bc" : u8"\u25bd";
                    } else {
                        triangle = entry->hasSubmodifiers() ? u8"\u25b6" : u8"\u25b7";
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

Qt::ItemFlags babelwires::FeatureModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    AccessModelScope scope(m_projectBridge);
    if (const FeatureElement* element = getFeatureElement(scope)) {
        if (const babelwires::ContentsCacheEntry* entry = getEntry(scope, index)) {
            const babelwires::UiProjectContext& context = m_projectBridge.getContext();
            RowModelDispatcher rowModel(context.m_rowModelReg, context.m_valueModelReg, entry, element);

            if (rowModel->isItemEditable()) {
                flags = flags | Qt::ItemIsEditable;
            }
        }
    }
    return flags;
}

QMenu* babelwires::FeatureModel::getContextMenu(const QModelIndex& index) {
    AccessModelScope scope(m_projectBridge);
    const FeatureElement* element = getFeatureElement(scope);
    if (!element) {
        return nullptr;
    }

    const babelwires::ContentsCacheEntry* entry = getEntry(scope, index);
    if (!entry) {
        return nullptr;
    }

    const babelwires::UiProjectContext& context = m_projectBridge.getContext();
    RowModelDispatcher rowModel(context.m_rowModelReg, context.m_valueModelReg, entry, element);

    std::vector<std::unique_ptr<FeatureContextMenuAction>> actions;
    rowModel->getContextMenuActions(actions);
    if (!actions.empty()) {
        FeatureContextMenu* menu = new FeatureContextMenu(*this, index);
        for (auto&& action : actions) {
            menu->addFeatureContextMenuAction(action.release());
        }
        return menu;
    }
    return nullptr;
}

babelwires::ProjectBridge& babelwires::FeatureModel::getProjectBridge() {
    return m_projectBridge;
}

babelwires::ElementId babelwires::FeatureModel::getElementId() const {
    return m_elementId;
}

void babelwires::FeatureModel::onClicked(const QModelIndex& index) const {
    const int column = index.column();
    if (column == 0) {
        AccessModelScope scope(m_projectBridge);
        const ContentsCacheEntry* entry = getEntry(scope, index);
        if (entry->isExpandable()) {
            const ElementId elementId = getElementId();
            const char* const actionName = entry->isExpanded() ? "Collapse compound" : "Expand compound";
            m_projectBridge.scheduleCommand(
                std::make_unique<SetExpandedCommand>(actionName, elementId, entry->getPath(), !entry->isExpanded()));
        }
    }
}
