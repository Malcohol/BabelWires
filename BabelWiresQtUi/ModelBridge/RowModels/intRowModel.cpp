/**
 * The row model for IntFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/intRowModel.hpp>

#include <BabelWiresQtUi/ValueEditors/spinBoxValueEditor.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Features/numericFeature.hpp>

#include <QString>

#include <cassert>

const babelwires::IntFeature& babelwires::IntRowModel::getIntFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::IntFeature>() && "Wrong type of feature stored");
    return *static_cast<const babelwires::IntFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::IntRowModel::getValueDisplayData() const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    const int value = intFeature.get();
    return SpinBoxValueEditor::getNamedValueString(intFeature.getValueNames(), value);
}

QWidget* babelwires::IntRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    auto spinBox = std::make_unique<SpinBoxValueEditor>(parent, index, intFeature.getValueNames());
    auto range = intFeature.getRange();
    spinBox->setRange(range.m_min, range.m_max);
    return spinBox.release();
}

void babelwires::IntRowModel::setEditorData(QWidget* editor) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    const int value = intFeature.get();
    auto spinBox = qobject_cast<SpinBoxValueEditor*>(editor);
    assert(spinBox && "Unexpected editor");
    spinBox->setValue(value);
}

std::unique_ptr<babelwires::ModifierData> babelwires::IntRowModel::createModifierFromEditor(QWidget* editor) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    int value = intFeature.get();
    const QSpinBox* spinBox = dynamic_cast<const QSpinBox*>(editor);
    assert(spinBox && "Unexpected editor");
    value = spinBox->value();
    if ((value != intFeature.get()) && intFeature.getRange().contains(value)) {
        auto modifier = std::make_unique<babelwires::IntValueAssignmentData>();
        modifier->m_pathToFeature = babelwires::FeaturePath(&intFeature);
        modifier->m_value = value;
        return modifier;
    } else {
        return nullptr;
    }
}

bool babelwires::IntRowModel::isItemEditable() const {
    return getInputFeature();
}
