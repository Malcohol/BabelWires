/**
 * RowModelDelegate is the QStyledItemDelegate responsible for the editing of entries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/rowModelDelegate.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModelDispatcher.hpp>
#include <BabelWiresQtUi/ValueEditors/valueEditorCommonBase.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <QApplication>
#include <QBrush>
#include <QComboBox>
#include <QPainter>

#include <cassert>

babelwires::RowModelDelegate::RowModelDelegate(QObject* parent, ProjectGraphModel& projectGraphModel)
    : QStyledItemDelegate(parent)
    , m_projectGraphModel(projectGraphModel) {}

QWidget* babelwires::RowModelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                                        const QModelIndex& index) const {
    const int column = index.column();
    if (column != 1) {
        return 0;
    }

    const NodeContentsModel* const model = dynamic_cast<const NodeContentsModel*>(index.model());
    assert(model && "Unexpected model");

    AccessModelScope scope(m_projectGraphModel);
    const Node* const node = model->getNode(scope);
    if (!node) {
        return nullptr;
    }

    const babelwires::ContentsCacheEntry* entry = model->getEntry(scope, index);
    if (!entry) {
        return nullptr;
    }
    const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable ValueTreeNode");
    QWidget* const editor = rowModel->createEditor(parent, index);

    if (editor) {
        ValueEditorInterface& interface = ValueEditorInterface::getValueEditorInterface(editor);
        // Update the editor if the model changes.
        interface.getValuesChangedConnection() = QObject::connect(
            model, &NodeContentsModel::valuesMayHaveChanged,
            [this, editor, parent, index]() { emit setEditorData(editor, index); });

        ValueEditorCommonSignals* ValueEditorCommonSignals = interface.getValueEditorSignals();
        // Update the model if the editor changes.
        QObject::connect(ValueEditorCommonSignals, &ValueEditorCommonSignals::editorHasChanged,
                    this, &RowModelDelegate::commitData);
    }

    if (!editor) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    return editor;
}

void babelwires::RowModelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    const NodeContentsModel* model = dynamic_cast<const NodeContentsModel*>(index.model());
    assert(model && "Unexpected model");

    AccessModelScope scope(m_projectGraphModel);
    const Node* node = model->getNode(scope);
    if (!node) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = model->getEntry(scope, index);
    if (!entry) {
        return;
    }

    const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable ValueTreeNode");
    rowModel->setEditorData(editor);

    ValueEditorInterface& interface = ValueEditorInterface::getValueEditorInterface(editor);
    interface.setIsModified(entry->hasModifier());
}

void babelwires::RowModelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                    const QModelIndex& index) const {
    AccessModelScope scope(m_projectGraphModel);
    NodeContentsModel* nodeContentsModel = dynamic_cast<NodeContentsModel*>(model);
    assert(model && "Unexpected model");

    const Node* node = nodeContentsModel->getNode(scope);
    if (!node) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = nodeContentsModel->getEntry(scope, index);
    if (!entry) {
        return;
    }
    const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
    RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);

    assert(rowModel->isItemEditable() && "We should not be trying to create an editor for a non-editable ValueTreeNode");
    // Allow the function to reject the contents of the editor.
    if (std::unique_ptr<Command<Project>> command = rowModel->createCommandFromEditor(editor)) {
        m_projectGraphModel.scheduleCommand(std::move(command));
    }
}

void babelwires::RowModelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                             const QModelIndex& index) const {
    const int column = index.column();
    if (column == 1) {
        const NodeContentsModel* nodeContentsModel = dynamic_cast<const NodeContentsModel*>(index.model());
        assert(nodeContentsModel && "Unexpected model");

        AccessModelScope scope(m_projectGraphModel);
        if (const Node* node = nodeContentsModel->getNode(scope)) {
            if (const babelwires::ContentsCacheEntry* entry = nodeContentsModel->getEntry(scope, index)) {
                const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
                RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);
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

QSize babelwires::RowModelDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const int column = index.column();
    if (column == 1) {
        const NodeContentsModel* nodeContentsModel = dynamic_cast<const NodeContentsModel*>(index.model());
        assert(nodeContentsModel && "Unexpected model");

        AccessModelScope scope(m_projectGraphModel);
        if (const Node* node = nodeContentsModel->getNode(scope)) {
            if (const babelwires::ContentsCacheEntry* entry = nodeContentsModel->getEntry(scope, index)) {
                const babelwires::UiProjectContext& context = m_projectGraphModel.getContext();
                RowModelDispatcher rowModel(context.m_valueModelReg, context.m_typeSystem, entry, node);
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

bool babelwires::RowModelDelegate::eventFilter(QObject* object, QEvent* event) {
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

void babelwires::RowModelDelegate::commitEditorValue() {
    QWidget* editor = qobject_cast<QWidget*>(sender());
    assert(editor && "This signal should not be hit with anything other than a widget");
    commitData(editor);
}
