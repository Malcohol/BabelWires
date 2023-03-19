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
    /*
    const babelwires::IntFeature& intFeature = getIntFeature();
    int value = intFeature.get();
    const QSpinBox* spinBox = dynamic_cast<const QSpinBox*>(editor);
    assert(spinBox && "Unexpected editor");
    value = spinBox->value();
    if ((value != intFeature.get()) && intFeature.getRange().contains(value)) {
        auto modifier = std::make_unique<babelwires::IntValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&intFeature);
        modifier->m_value = value;
        return std::make_unique<AddModifierCommand>("Set int value", m_featureElement->getElementId(),
                                            std::move(modifier));
    } else {
        return nullptr;
    }
    */
   return nullptr;
}

bool babelwires::ValueRowModel::isItemEditable() const {
    return getInputFeature();
}
