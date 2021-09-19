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

unsigned int babelwires::Enum::getIndexFromIdentifier(babelwires::FieldIdentifier id) const {
    const EnumValues& values = getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    assert((it != values.end()) && "id not found in enum");
    return it - values.begin();
}

babelwires::FieldIdentifier babelwires::Enum::getIdentifierFromIndex(unsigned int index) {
    const EnumValues& values = getEnumValues();
    assert(index < values.size());
    return values[index];
}

babelwires::EnumRegistry::EnumRegistry()
    : Registry("Enum registry") {}