/**
 * Model for IntValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/intValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/spinBoxValueEditor.hpp>

#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

namespace {
    // QSpinBox cannot cover the full range of IntValue::NativeType, but it's likely to be rare for someone
    // to want to type in a really really big number.
    int clampToInt(babelwires::IntValue::NativeType value) {
        return static_cast<int>(std::clamp(value, static_cast<babelwires::IntValue::NativeType>(std::numeric_limits<int>::min()),
                                         static_cast<babelwires::IntValue::NativeType>(std::numeric_limits<int>::max())));
    }
}

QWidget* babelwires::IntValueModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    auto spinBox = std::make_unique<SpinBoxValueEditor>(parent, index, nullptr);
    auto range = getType()->is<IntType>().getRange();
    spinBox->setMinimum(clampToInt(range.m_min));
    spinBox->setMaximum(clampToInt(range.m_max));
    return spinBox.release();
}

void babelwires::IntValueModel::setEditorData(QWidget* editor) const {
    const IntValue& v = getValue()->is<IntValue>();
    const int value = v.get();

    auto spinBox = qobject_cast<SpinBoxValueEditor*>(editor);
    assert(spinBox && "Unexpected editor");
    spinBox->setValue(value);
}

babelwires::EditableValueHolder babelwires::IntValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    const QSpinBox* spinBox = dynamic_cast<const QSpinBox*>(editor);
    assert(spinBox && "Unexpected editor");
    const int newValue = spinBox->value();

    const IntValue& v = getValue()->is<IntValue>();
    const int currentValue = v.get();

    if (newValue != currentValue) {
        return EditableValueHolder::makeValue<babelwires::IntValue>(newValue);
    }
    return {};
}

bool babelwires::IntValueModel::isItemEditable() const {
    return getValue()->as<IntValue>();
}

bool babelwires::IntValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<SpinBoxValueEditor*>(editor);
}
