/**
 * Dispatch to the appropriate value model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/valueModelDispatcher.hpp>

#include <BabelWiresQtUi/ValueModels/enumValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/intValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/stringValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/rationalValueModel.hpp>
#include <BabelWiresQtUi/ValueModels/valueModelRegistry.hpp>

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>

void babelwires::ValueModelDispatcher::init(const ValueModelRegistry& valueModelRegistry, const Type& type, const Value& value) {
    m_valueModel = &m_valueModelStorage;
    if (valueModelRegistry.handleFeature(&type, m_valueModel)) {
        // Handled by a registered handler.
    } else if (type.as<Enum>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::EnumValueModel));
        new (m_valueModel) babelwires::EnumValueModel();
    } else if (type.as<IntType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::IntValueModel));
        new (m_valueModel) babelwires::IntValueModel();
    } else if (type.as<RationalType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::RationalValueModel));
        new (m_valueModel) babelwires::RationalValueModel();
    } else if (type.as<StringType>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::StringValueModel));
        new (m_valueModel) babelwires::StringValueModel();
    } else {
        // The base row model is used.
    }
    m_valueModel->m_type = &type;
    m_valueModel->m_value = &value;
}
