/**
 * Base class of models for values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModel.hpp>

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

QWidget* babelwires::ValueModel::createEditor(const QModelIndex& index, QWidget* parent) const {
    assert(false && "This method must be overridden in subclasses");
    return nullptr;
}

void babelwires::ValueModel::setEditorData(QWidget* editor) const {
    assert(false && "This method must be overridden in subclasses");
}

std::unique_ptr<babelwires::Value> babelwires::ValueModel::createValueFromEditorIfDifferent(QWidget* editor) const {
    assert(false && "This method must be overridden in subclasses");
    return {};
}

bool babelwires::ValueModel::isItemEditable() const {
    return false;
}