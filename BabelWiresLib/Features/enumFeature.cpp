/**
 * An EnumFeature exposes an enum in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/enumFeature.hpp"

#include "BabelWiresLib/Enums/enum.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"
#include "BabelWiresLib/Features/Path/fieldNameRegistry.hpp"

babelwires::EnumFeature::EnumFeature(const Enum& e)
    : m_enum(e)
    , m_value(e.getEnumValues()[e.getIndexOfDefaultValue()]) {}

const babelwires::Enum& babelwires::EnumFeature::getEnum() const {
    return m_enum;
}

babelwires::FieldIdentifier babelwires::EnumFeature::get() const {
    return m_value;
}

void babelwires::EnumFeature::set(FieldIdentifier id) {
    const Enum::EnumValues& values = m_enum.getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    if (it != values.end()) {
        m_value = id;
    }
    else {
        throw ModelException() << "The value \"" << FieldNameRegistry::read()->getFieldName(id) << "\" is not a valid value for the enum \"" << m_enum.getName() << "\" enum.";
    }
    m_value = id;
}

void babelwires::EnumFeature::doSetToDefault() {
    m_value = m_enum.getEnumValues()[m_enum.getIndexOfDefaultValue()];
}

std::size_t babelwires::EnumFeature::doGetHash() const {
    return std::hash<FieldIdentifier>{}(m_value);
}

std::string babelwires::EnumFeature::doGetValueType() const {
    return "enum";
}

void babelwires::EnumFeature::doAssign(const ValueFeature& other) {
    const EnumFeature& otherEnum = static_cast<const EnumFeature&>(other);
    FieldIdentifier otherValue = otherEnum.get();
    set(otherValue);
}
