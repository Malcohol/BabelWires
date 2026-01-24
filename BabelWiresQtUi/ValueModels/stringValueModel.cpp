/**
 * Model for StringValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/stringValueModel.hpp>

#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

QWidget* babelwires::StringValueModel::createEditor(QWidget* parent) const {
    return new LineEditValueEditor(parent);
}

void babelwires::StringValueModel::setEditorData(QWidget* editor) const {
    const StringValue& v = getValue()->as<StringValue>();
    const std::string& value = v.get();

    auto lineEditor = qobject_cast<LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    lineEditor->setText(value.c_str());
}

babelwires::ValueHolder babelwires::StringValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    auto lineEditor = qobject_cast<const LineEditValueEditor*>(editor);
    assert(lineEditor && "Unexpected editor");
    const std::string newValue = lineEditor->text().toStdString();

    const StringValue& v = getValue()->as<StringValue>();
    const std::string currentValue = v.get();
    
    if (newValue != currentValue) {
        return ValueHolder::makeValue<babelwires::StringValue>(newValue);
    } 
    return {};
}

bool babelwires::StringValueModel::isItemEditable() const {
    return getValue()->tryAs<StringValue>();
}

bool babelwires::StringValueModel::validateEditor(QWidget* editor) const {
    return qobject_cast<LineEditValueEditor*>(editor);
}
