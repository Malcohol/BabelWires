/**
 * Base class of models for values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/valueModel.hpp>

#include <BabelWiresLib/TypeSystem/value.hpp>

QVariant babelwires::ValueModel::getDisplayData() const {
    return QString(m_value->toString().c_str());
}

const babelwires::Value* babelwires::ValueModel::getValue() const {
    return m_value;
}

const babelwires::Type* babelwires::ValueModel::getType() const {
    return m_type;
}

QWidget* babelwires::ValueModel::createEditor(QWidget* parent, const QModelIndex& index) const {
    assert(false && "This method must be overridden in subclasses");
    return nullptr;
}

void babelwires::ValueModel::setEditorData(QWidget* editor) const {
    assert(false && "This method must be overridden in subclasses");
}

babelwires::EditableValueHolder babelwires::ValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    assert(false && "This method must be overridden in subclasses");
    return {};
}

bool babelwires::ValueModel::isItemEditable() const {
    return false;
}

bool babelwires::ValueModel::validateEditor(QWidget* editor) const {
    return false;
}