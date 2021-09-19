#include "BabelWiresLib/Enums/enum.hpp"

babelwires::Enum::Enum(std::string identifier, std::string name, VersionNumber version, const EnumValues& values,
                       unsigned int indexOfDefaultValue)
    : RegistryEntry(std::move(identifier), std::move(name), version)
    , m_values(values)
    , m_indexOfDefaultValue(indexOfDefaultValue) {}

const babelwires::Enum::EnumValues& babelwires::Enum::getEnumValues() const {
    return m_values;
}

unsigned int babelwires::Enum::getIndexOfDefaultValue() const {
    return m_indexOfDefaultValue;
}

babelwires::EnumRegistry::EnumRegistry()
    : Registry("Enum registry") {}