/**
 * The row model for IntFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/intRowModel.hpp"

#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/ValueNames/valueNames.hpp"
#include "BabelWiresQtUi/ModelBridge/ValueEditors/comboBoxValueEditor.hpp"
#include "BabelWiresQtUi/ModelBridge/ValueEditors/spinBoxValueEditor.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include <QString>

#include <cassert>

const babelwires::IntFeature& babelwires::IntRowModel::getIntFeature() const {
    assert(dynamic_cast<const babelwires::IntFeature*>(getInputThenOutputFeature()) && "Wrong type of feature stored");
    return *static_cast<const babelwires::IntFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::IntRowModel::getValueDisplayData() const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    const int value = intFeature.get();
    if (const babelwires::ValueNames* valueNames = intFeature.getValueNames()) {
        std::string name;
        if (valueNames->getNameForValue(value, name)) {
            return QString("%1 (%2)").arg(name.c_str()).arg(value);
        }
    }
    return QString("%1").arg(value);
}

QWidget* babelwires::IntRowModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    if (const babelwires::ValueNames* valueNames = intFeature.getValueNames()) {
        const auto range = intFeature.getRange();
        auto comboBox = std::make_unique<ComboBoxValueEditor>(parent, index);
        comboBox->setEditable(true);
        for (auto valueName : *valueNames) {
            if (range.contains(std::get<0>(valueName))) {
                comboBox->addItem(std::get<1>(valueName).c_str());
            }
        }
        return comboBox.release();
    } else {
        auto spinBox = std::make_unique<SpinBoxValueEditor>(parent, index);
        auto range = intFeature.getRange();
        spinBox->setRange(range.m_min, range.m_max);
        return spinBox.release();
    }
}

void babelwires::IntRowModel::setEditorData(QWidget* editor) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    const int value = intFeature.get();
    if (const babelwires::ValueNames* valueNames = intFeature.getValueNames()) {
        auto comboBox = qobject_cast<ComboBoxValueEditor*>(editor);
        assert(comboBox && "Unexpected editor");
        std::string name;
        if (valueNames->getNameForValue(value, name)) {
            comboBox->setCurrentText(QString(name.c_str()));
        } else {
            comboBox->setCurrentText(QString::number(value));
        }
    } else {
        auto spinBox = qobject_cast<SpinBoxValueEditor*>(editor);
        assert(spinBox && "Unexpected editor");
        spinBox->setValue(value);
    }
}

std::unique_ptr<babelwires::ModifierData> babelwires::IntRowModel::createModifierFromEditor(QWidget* editor) const {
    const babelwires::IntFeature& intFeature = getIntFeature();
    int value = intFeature.get();
    if (const babelwires::ValueNames* valueNames = intFeature.getValueNames()) {
        auto comboBox = dynamic_cast<ComboBoxValueEditor*>(editor);
        assert(comboBox && "Unexpected editor");
        bool success;
        int newValue = comboBox->currentText().toInt(&success);
        if (success) {
            value = newValue;
        } else if (valueNames->getValueForName(comboBox->currentText().toStdString(), newValue)) {
            value = newValue;
        }
    } else {
        const QSpinBox* spinBox = dynamic_cast<const QSpinBox*>(editor);
        assert(spinBox && "Unexpected editor");
        value = spinBox->value();
    }
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
