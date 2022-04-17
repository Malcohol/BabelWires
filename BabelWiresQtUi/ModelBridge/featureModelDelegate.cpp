/**
 * FeatureModelDelegate is the QStyledItemDelegate responsible for the editing of entries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp"

#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"
#include "BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp"
#include "BabelWiresQtUi/ModelBridge/ValueEditors/valueEditorCommonBase.hpp"
#include "BabelWiresQtUi/ModelBridge/accessModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/modifyModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/uiProjectContext.hpp"

#include "BabelWiresLib/Commands/commands.hpp"
#include "BabelWiresLib/Project/Commands/addModifierCommand.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"

#include <QApplication>
#include <QBrush>
#include <QComboBox>
#include <QPainter>

#include <cassert>

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
    QWidget* const editor = rowModel->createEditor(parent, index);

    if (editor) {
        QVariant property = editor->property(ValueEditorInterface::s_propertyName);
        if (property.isValid()) {
            ValueEditorInterface* interface = qvariant_cast<ValueEditorInterface*>(property);

            // Update the editor if the model changes.
            interface->getValuesChangedConnection() = QObject::connect(
                RowModel::getModelFromParentWidget(parent), &FeatureModel::valuesMayHaveChanged,
                [editor, parent, index]() { emit RowModel::getDelegateFromParentWidget(parent)->setEditorData(editor, index); });
        }
    }

    if (!editor) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    return editor;
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
