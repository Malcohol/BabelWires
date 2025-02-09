/**
 * Model for IntValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/tupleValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/tupleValueEditor.hpp>

#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

/*
QWidget* babelwires::TupleValueModel::createEditor(QWidget* parent) const {
    auto spinBox = std::make_unique<TupleValueEditor>(parent);
    const TupleType& tupleType = getType()->is<TupleType>();
    return spinBox.release();
}

void babelwires::TupleValueModel::setEditorData(QWidget* editor) const {
    const auto& v = getValue()->is<TupleValue>();
    const int value = v.get();

    auto spinBox = qobject_cast<SpinBoxValueEditor*>(editor);
    assert(spinBox && "Unexpected editor");
    spinBox->setValue(value);
}

babelwires::EditableValueHolder babelwires::TupleValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
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

bool babelwires::TupleValueModel::isItemEditable() const {
    return getValue()->as<TupleValue>();
}

bool babelwires::TupleValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<SpinBoxValueEditor*>(editor);
}
*/