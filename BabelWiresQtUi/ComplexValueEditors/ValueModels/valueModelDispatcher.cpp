/**
 * Dispatch to the appropriate value model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/valueModelDispatcher.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/ValueModels/enumValueModel.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

babelwires::ValueModelDispatcher::ValueModelDispatcher(const Type& type, const Value& value) {
    m_valueModel = &m_valueModelStorage;
    if (type.as<Enum>()) {
        static_assert(sizeof(babelwires::ValueModel) == sizeof(babelwires::EnumValueModel));
        new (m_valueModel) babelwires::EnumValueModel();
    } else {
        // The base row model is used.
    }
    m_valueModel->m_type = &type;
    m_valueModel->m_value = &value;
}
