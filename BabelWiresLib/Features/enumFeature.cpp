/**
 * An EnumFeature exposes an enum in the project, and allows the user to make a choice from a set of named values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/enumFeature.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::EnumFeature::EnumFeature(TypeRef e)
    : m_enum(std::move(e)) {}

const babelwires::Enum& babelwires::EnumFeature::getEnum() const {
    const ProjectContext& projectContext = RootFeature::getProjectContextAt(*this);
    const Type* type = m_enum.tryResolve(projectContext.m_typeSystem);
    assert(type && "The enum to set is not of a known type");
    const Enum* e = type->as<Enum>();
    assert(type && "The type of the enum to set is not an enum");
    return *e;
}

babelwires::ShortId babelwires::EnumFeature::get() const {
    return m_value;
}

void babelwires::EnumFeature::set(ShortId id) {
    const Enum& e = getEnum();
    const Enum::EnumValues& values = e.getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    if (it == values.end()) {
        throw ModelException() << "The value \"" << IdentifierRegistry::read()->getName(id)
                               << "\" is not a valid value for the enum \"" << e.getName() << "\" enum.";
    }
    if (id != m_value) {
        setChanged(Changes::ValueChanged);
        m_value = id;
    }
}

void babelwires::EnumFeature::doSetToDefault() {
    const Enum& e = getEnum();
    const ShortId defaultValue = e.getIdentifierFromIndex(e.getIndexOfDefaultValue());
    if (defaultValue != m_value) {
        setChanged(Changes::ValueChanged);
        m_value = defaultValue;
    }
}

std::size_t babelwires::EnumFeature::doGetHash() const {
    return std::hash<ShortId>{}(m_value);
}

std::string babelwires::EnumFeature::doGetValueType() const {
    return "enum";
}

void babelwires::EnumFeature::doAssign(const ValueFeature& other) {
    const EnumFeature& otherEnum = static_cast<const EnumFeature&>(other);
    ShortId otherValue = otherEnum.get();
    set(otherValue);
}
