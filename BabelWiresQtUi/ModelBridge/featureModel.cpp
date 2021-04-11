/**
 * FeatureModel is the QAbstractTableModel which represents the data in a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp"
#include "BabelWiresQtUi/ModelBridge/ValueEditors/valueEditorCommonBase.hpp"
#include "BabelWiresQtUi/ModelBridge/accessModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/modifyModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/Utilities/fileDialogs.hpp"
#include "BabelWiresQtUi/uiProjectContext.hpp"

#include "BabelWires/Commands/addModifierCommand.hpp"
#include "BabelWires/Commands/commandManager.hpp"
#include "BabelWires/Commands/commands.hpp"
#include "BabelWires/Commands/setExpandedCommand.hpp"
#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWires/FileFormat/fileFormat.hpp"
#include "BabelWires/Project/FeatureElements/fileElement.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWires/Project/projectContext.hpp"
#include "BabelWires/Project/uiPosition.hpp"
#include "BabelWires/ValueNames/valueNames.hpp"

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
    RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

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
                    return QString(entry->getPath().getNumSteps() * 2 - 1, ' ') + triangle + entry->getLabel().c_str();
                } else {
                    return QString(entry->getPath().getNumSteps() * 2, ' ') + entry->getLabel().c_str();
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
            RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

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

    RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

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

babelwires::FeatureModelDelegate::FeatureModelDelegate(QObject* parent, ProjectBridge& projectBridge)
    : QStyledItemDelegate(parent)
    , m_projectBridge(projectBridge) {}

QWidget* babelwires::FeatureModelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                                        const QModelIndex& index) const {
    const int column = index.column();
    if (column != 1) {
        return 0;
    }

    const FeatureModel* const model = dynamic_cast<const FeatureModel*>(index.model());
    assert(model && "Unexpected model");

    AccessModelScope scope(m_projectBridge);
    const FeatureElement* const element = model->getFeatureElement(scope);
    if (!element) {
        return nullptr;
    }

    const babelwires::ContentsCacheEntry* entry = model->getEntry(scope, index);
    if (!entry) {
        return nullptr;
    }
    RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable feature");
    QWidget* result = rowModel->createEditor(parent, index);

    if (!result) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    return result;
}

void babelwires::FeatureModelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    const FeatureModel* model = dynamic_cast<const FeatureModel*>(index.model());
    assert(model && "Unexpected model");

    AccessModelScope scope(m_projectBridge);
    const FeatureElement* element = model->getFeatureElement(scope);
    if (!element) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = model->getEntry(scope, index);
    if (!entry) {
        return;
    }

    RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable feature");
    rowModel->setEditorData(editor);

    // Set the editor to modified if the editor carries the ValueEditorInterface property.
    QVariant property = editor->property(ValueEditorInterface::s_propertyName);
    if (property.isValid()) {
        ValueEditorInterface* interface = qvariant_cast<ValueEditorInterface*>(property);
        interface->setFeatureIsModified(entry->hasModifier());
    }
}

void babelwires::FeatureModelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                    const QModelIndex& index) const {
    AccessModelScope scope(m_projectBridge);
    FeatureModel* featureModel = dynamic_cast<FeatureModel*>(model);
    assert(model && "Unexpected model");

    const FeatureElement* element = featureModel->getFeatureElement(scope);
    if (!element) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = featureModel->getEntry(scope, index);
    if (!entry) {
        return;
    }
    RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable feature");
    // Allow the function to reject the contents of the editor.
    if (std::unique_ptr<ModifierData> modifier = rowModel->createModifierFromEditor(editor)) {
        m_projectBridge.scheduleCommand(
            std::make_unique<AddModifierCommand>("Set value", element->getElementId(), std::move(modifier)));
    }
}

void babelwires::FeatureModelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                             const QModelIndex& index) const {
    const int column = index.column();
    if (column == 1) {
        const FeatureModel* featureModel = dynamic_cast<const FeatureModel*>(index.model());
        assert(featureModel && "Unexpected model");

        AccessModelScope scope(m_projectBridge);
        if (const FeatureElement* element = featureModel->getFeatureElement(scope)) {
            if (const babelwires::ContentsCacheEntry* entry = featureModel->getEntry(scope, index)) {
                RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);
                if (rowModel->hasCustomPainting()) {
                    QStyleOptionViewItem options = option;
                    initStyleOption(&options, index);
                    painter->save();
                    rowModel->paint(painter, options, index);
                    painter->restore();
                    return;
                }
            }
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QSize babelwires::FeatureModelDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const int column = index.column();
    if (column == 1) {
        const FeatureModel* featureModel = dynamic_cast<const FeatureModel*>(index.model());
        assert(featureModel && "Unexpected model");

        AccessModelScope scope(m_projectBridge);
        if (const FeatureElement* element = featureModel->getFeatureElement(scope)) {
            if (const babelwires::ContentsCacheEntry* entry = featureModel->getEntry(scope, index)) {
                RowModelDispatcher rowModel(m_projectBridge.getContext().m_rowModelReg, entry, element);
                if (rowModel->hasCustomPainting()) {
                    QStyleOptionViewItem options = option;
                    initStyleOption(&options, index);
                    return rowModel->sizeHint(options, index);
                }
            }
        }
    }

    return QStyledItemDelegate::sizeHint(option, index);
}

bool babelwires::FeatureModelDelegate::eventFilter(QObject* object, QEvent* event) {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(object);
    if (comboBox) {
        if (event->type() == QEvent::MouseButtonRelease) {
            // Hack: ensure the popup is shown.
            comboBox->showPopup();
            return true;
        }
    } else {
        return QStyledItemDelegate::eventFilter(object, event);
    }
    return false;
}

void babelwires::FeatureModelDelegate::commitEditorValue() {
    QWidget* editor = qobject_cast<QWidget*>(sender());
    assert(editor && "This signal should not be hit with anything other than a widget");
    commitData(editor);
}
