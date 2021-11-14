/**
 * An EnumFeature exposes an enum in the project, and allows the user to make a choice from a set of named values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/enumFeature.hpp"

#include "BabelWiresLib/Enums/enum.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"
#include "Common/Identifiers/identifierRegistry.hpp"

babelwires::EnumFeature::EnumFeature(const Enum& e)
    : m_enum(e)
    , m_value(e.getEnumValues()[e.getIndexOfDefaultValue()]) {}

const babelwires::Enum& babelwires::EnumFeature::getEnum() const {
    return m_enum;
}

babelwires::Identifier babelwires::EnumFeature::get() const {
    return m_value;
}

void babelwires::EnumFeature::set(Identifier id) {
    const Enum::EnumValues& values = m_enum.getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    if (it == values.end()) {
        throw ModelException() << "The value \"" << IdentifierRegistry::read()->getName(id) << "\" is not a valid value for the enum \"" << m_enum.getName() << "\" enum.";
    }
    if (id != m_value) {
        setChanged(Changes::ValueChanged);
        m_value = id;
    }
}

void babelwires::EnumFeature::doSetToDefault() {
    m_value = m_enum.getEnumValues()[m_enum.getIndexOfDefaultValue()];
}

std::size_t babelwires::EnumFeature::doGetHash() const {
    return std::hash<Identifier>{}(m_value);
}

std::string babelwires::EnumFeature::doGetValueType() const {
    return "enum";
}

void babelwires::EnumFeature::doAssign(const ValueFeature& other) {
    const EnumFeature& otherEnum = static_cast<const EnumFeature&>(other);
    Identifier otherValue = otherEnum.get();
    set(otherValue);
}
