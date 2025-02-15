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

QWidget* babelwires::TupleValueModel::createEditor(QWidget* parent) const {
    const auto& tupleType = getType()->is<TupleType>();
    const auto& tupleValue = getValue()->is<TupleValue>();
    auto tupleEditor = std::make_unique<TupleValueEditor>(parent, *m_valueModelRegistry, *m_typeSystem, tupleType, tupleValue);
    return tupleEditor.release();
}


void babelwires::TupleValueModel::setEditorData(QWidget* editor) const {
    auto tupleEditor = qobject_cast<TupleValueEditor*>(editor);
    assert(tupleEditor && "Unexpected editor");
    const auto& tupleValue = getValue()->is<TupleValue>();
    tupleEditor->setEditorData(tupleValue);
}

babelwires::EditableValueHolder babelwires::TupleValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    auto tupleEditor = dynamic_cast<const TupleValueEditor*>(editor);
    assert(tupleEditor && "Unexpected editor");

    const auto& currentValue = getValue()->is<TupleValue>();
    const auto& editorValue = tupleEditor->getEditorData();

    if (editorValue != currentValue) {
        return editorValue;
    }
    return {};
}

bool babelwires::TupleValueModel::isItemEditable() const {
    return getValue()->as<TupleValue>();
}

bool babelwires::TupleValueModel::validateEditor(QWidget* editor) const {
    if (const auto* tupleEditor = qobject_cast<const TupleValueEditor*>(editor)) {
        return (&tupleEditor->getType() == getType());
    }
    return false;
}
