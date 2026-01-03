/**
 * The row model for ValueTreeNodes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>

#include <QString>

#include <cassert>

void babelwires::ValueRowModel::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem) {
    const babelwires::ValueTreeNode& valueTreeNode = getValueTreeNode();
    m_valueModelDispatcher.init(valueModelRegistry, typeSystem, valueTreeNode.getTypeExp().resolve(typeSystem), valueTreeNode.getValue(),
                                (getInput() == nullptr),
                                m_contentsCacheEntry->isStructureEditable());
}

const babelwires::ValueTreeNode& babelwires::ValueRowModel::getValueTreeNode() const {
    return *getOutputThenInput();
}

QVariant babelwires::ValueRowModel::getValueDisplayData() const {
    const ValueModel::StyleHint styleHint =
        isModified() ? ValueModel::StyleHint::Bold : ValueModel::StyleHint::Normal;
    return m_valueModelDispatcher->getDisplayData(styleHint);
}

QWidget* babelwires::ValueRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    return m_valueModelDispatcher->createEditor(parent);
}

void babelwires::ValueRowModel::setEditorData(QWidget* editor) const {
    m_valueModelDispatcher->setEditorData(editor);
}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ValueRowModel::createCommandFromEditor(QWidget* editor) const {
    if (ValueHolder newValue = m_valueModelDispatcher->createValueFromEditorIfDifferent(editor)) {
        const babelwires::ValueTreeNode& valueTreeNode = getValueTreeNode();
        auto modifier = std::make_unique<babelwires::ValueAssignmentData>(std::move(newValue));
        modifier->m_targetPath = babelwires::getPathTo(&valueTreeNode);
        return std::make_unique<AddModifierCommand>("Set value", m_node->getNodeId(), std::move(modifier));
    }
    return nullptr;
}

bool babelwires::ValueRowModel::isItemEditable() const {
    if (getInput()) {
        return m_valueModelDispatcher->isItemEditable();
    }
    return false;
}

bool babelwires::ValueRowModel::hasCustomPainting() const {
    return m_valueModelDispatcher->hasCustomPainting();
}

void babelwires::ValueRowModel::paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const {
    m_valueModelDispatcher->paint(painter, option);
}

QSize babelwires::ValueRowModel::sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const {
    return m_valueModelDispatcher->sizeHint(option);
}

QString babelwires::ValueRowModel::getTooltip(ColumnType c) const {
    const QString rowTooltip = RowModel::getTooltip(c);
    QString columnTooltip;
    if (c == ColumnType::Key) {
        const babelwires::ValueTreeNode& valueTreeNode = getValueTreeNode();
        columnTooltip = QString("%1 : %2").arg(m_contentsCacheEntry->getLabel().c_str()).arg(valueTreeNode.getType()->getName().c_str());
    } else {
        columnTooltip = m_valueModelDispatcher->getTooltip();
    } 
    if (rowTooltip.isEmpty() || columnTooltip.isEmpty()) {
        return rowTooltip + columnTooltip;
    } else {
        return rowTooltip + "\n" + columnTooltip;
    }
}

void babelwires::ValueRowModel::getContextMenuActions(
    std::vector<ContextMenuEntry>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    const babelwires::ValueTreeNode& valueTreeNode = getValueTreeNode();
    m_valueModelDispatcher->getContextMenuActions(
        ProjectDataLocation{m_node->getNodeId(), babelwires::getPathTo(&valueTreeNode)}, actionsOut);
}
