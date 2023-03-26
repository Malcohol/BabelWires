/**
 * Model for IntValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/intValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/spinBoxValueEditor.hpp>

#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

QWidget* babelwires::IntValueModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    // TODO value names
    auto spinBox = std::make_unique<SpinBoxValueEditor>(parent, index, nullptr);
    // TODO range
    return spinBox.release();
}

void babelwires::IntValueModel::setEditorData(QWidget* editor) const {
    const IntValue& v = getValue()->is<IntValue>();
    const int value = v.get();

    auto spinBox = qobject_cast<SpinBoxValueEditor*>(editor);
    assert(spinBox && "Unexpected editor");
    spinBox->setValue(value);
}

std::unique_ptr<babelwires::Value> babelwires::IntValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    const QSpinBox* spinBox = dynamic_cast<const QSpinBox*>(editor);
    assert(spinBox && "Unexpected editor");
    const int newValue = spinBox->value();

    const IntValue& v = getValue()->is<IntValue>();
    const int currentValue = v.get();

    if (newValue != currentValue) {
        auto newIntValue = std::make_unique<babelwires::IntValue>();
        newIntValue->set(newValue);
        return newIntValue;
    }
    return {};
}

bool babelwires::IntValueModel::isItemEditable() const {
    return m_value->as<IntValue>();
}

bool babelwires::IntValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<SpinBoxValueEditor*>(editor);
}
