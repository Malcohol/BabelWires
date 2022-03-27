/**
 * Base class of models for values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModel.hpp>

QVariant babelwires::ValueModel::getDisplayData() const {
    return {};
}

const babelwires::Value* babelwires::ValueModel::getValue() const {
    return m_value;
}

const babelwires::Type* babelwires::ValueModel::getType() const {
    return m_type;
}
