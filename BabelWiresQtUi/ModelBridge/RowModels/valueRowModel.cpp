/**
 * The row model for ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <QString>

#include <cassert>

void babelwires::ValueRowModel::init(const ValueModelRegistry& valueModelRegistry) {
    const babelwires::ValueFeature& valueFeature = getValueFeature();
    m_valueModelDispatcher.init(valueModelRegistry, valueFeature.getType(), *valueFeature.getValue(),
                                (getInputFeature() == nullptr),
                                m_contentsCacheEntry->isStructureEditable());
}

const babelwires::ValueFeature& babelwires::ValueRowModel::getValueFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::ValueFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::ValueFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::ValueRowModel::getValueDisplayData() const {
    const ValueModel::StyleHint styleHint =
        isFeatureModified() ? ValueModel::StyleHint::Bold : ValueModel::StyleHint::Normal;
    return m_valueModelDispatcher->getDisplayData(styleHint);
}

QWidget* babelwires::ValueRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    return m_valueModelDispatcher->createEditor(parent, index);
}

void babelwires::ValueRowModel::setEditorData(QWidget* editor) const {
    m_valueModelDispatcher->setEditorData(editor);
}

std::unique_ptr<babelwires::Command<babelwires::Project>>
babelwires::ValueRowModel::createCommandFromEditor(QWidget* editor) const {
    if (EditableValueHolder newValue = m_valueModelDispatcher->createValueFromEditorIfDifferent(editor)) {
        const babelwires::ValueFeature& valueFeature = getValueFeature();
        auto modifier = std::make_unique<babelwires::ValueAssignmentData>(std::move(newValue));
        modifier->m_pathToFeature = babelwires::FeaturePath(&valueFeature);
        return std::make_unique<AddModifierCommand>("Set value", m_featureElement->getElementId(), std::move(modifier));
    }
    return nullptr;
}

bool babelwires::ValueRowModel::isItemEditable() const {
    if (getInputFeature()) {
        return m_valueModelDispatcher->isItemEditable();
    }
    return false;
}

bool babelwires::ValueRowModel::hasCustomPainting() const {
    return m_valueModelDispatcher->hasCustomPainting();
}

void babelwires::ValueRowModel::paint(QPainter* painter, QStyleOptionViewItem& option, const QModelIndex& index) const {
    m_valueModelDispatcher->paint(painter, option, index);
}

QSize babelwires::ValueRowModel::sizeHint(QStyleOptionViewItem& option, const QModelIndex& index) const {
    return m_valueModelDispatcher->sizeHint(option, index);
}

QString babelwires::ValueRowModel::getTooltip() const {
    QString rowTooltip = RowModel::getTooltip();
    QString valueTooltip = m_valueModelDispatcher->getTooltip();
    if (rowTooltip.isEmpty() || valueTooltip.isEmpty()) {
        return rowTooltip + valueTooltip;
    } else {
        return rowTooltip + "/n" + valueTooltip;
    }
}

void babelwires::ValueRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    const babelwires::ValueFeature& valueFeature = getValueFeature();
    m_valueModelDispatcher->getContextMenuActions(
        DataLocation{m_featureElement->getElementId(), babelwires::FeaturePath(&valueFeature)}, actionsOut);
}
