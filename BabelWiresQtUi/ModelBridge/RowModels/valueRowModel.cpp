/**
 * The row model for IntFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/valueRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <QString>

#include <cassert>

void babelwires::ValueRowModel::init() {
    const babelwires::SimpleValueFeature& valueFeature = getValueFeature();
    m_valueModelDispatcher.init(valueFeature.getType(), valueFeature.getValue());
}

const babelwires::SimpleValueFeature& babelwires::ValueRowModel::getValueFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::SimpleValueFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::SimpleValueFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::ValueRowModel::getValueDisplayData() const {
    return m_valueModelDispatcher->getDisplayData();
}

QWidget* babelwires::ValueRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    return m_valueModelDispatcher->createEditor(parent, index);
}

void babelwires::ValueRowModel::setEditorData(QWidget* editor) const {
    m_valueModelDispatcher->setEditorData(editor);
}

std::unique_ptr<babelwires::Command<babelwires::Project>> babelwires::ValueRowModel::createCommandFromEditor(QWidget* editor) const {
    if (std::unique_ptr<Value> newValue = m_valueModelDispatcher->createValueFromEditorIfDifferent(editor)) {
        const babelwires::SimpleValueFeature& valueFeature = getValueFeature();
        auto modifier = std::make_unique<babelwires::ValueAssignmentData>(std::move(newValue));
        modifier->m_pathToFeature = babelwires::FeaturePath(&valueFeature);
        return std::make_unique<AddModifierCommand>("Set value", m_featureElement->getElementId(),
                                            std::move(modifier));
    }
    return nullptr;
}

bool babelwires::ValueRowModel::isItemEditable() const {
    return getInputFeature();
}