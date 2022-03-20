#include "BabelWiresLib/Enums/enum.hpp"

#include <BabelWiresLib/TypeSystem/enumValue.hpp>

babelwires::Enum::Enum(LongIdentifier identifier, VersionNumber version, const EnumValues& values,
                       unsigned int indexOfDefaultValue)
    : Type(identifier, version)
    , m_values(values)
    , m_indexOfDefaultValue(indexOfDefaultValue) {
#ifndef NDEBUG
    for (int i = 0; i < values.size(); ++i) {
        assert((values[i].getDiscriminator() != 0) && "Only registered ids can be used in an enum");
    }
#endif
    }

const babelwires::Enum::EnumValues& babelwires::Enum::getEnumValues() const {
    return m_values;
}

unsigned int babelwires::Enum::getIndexOfDefaultValue() const {
    return m_indexOfDefaultValue;
}

unsigned int babelwires::Enum::getIndexFromIdentifier(babelwires::Identifier id) const {
    const EnumValues& values = getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    assert((it != values.end()) && "id not found in enum");
    return it - values.begin();
}

babelwires::Identifier babelwires::Enum::getIdentifierFromIndex(unsigned int index) const {
    const EnumValues& values = getEnumValues();
    assert(index < values.size());
    return values[index];
}

bool babelwires::Enum::isAValue(babelwires::Identifier id) const {
    const EnumValues& values = getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    return it != values.end();
}

std::unique_ptr<babelwires::Value> babelwires::Enum::createValue() const {
    return std::make_unique<EnumValue>(getIdentifierFromIndex(getIndexOfDefaultValue()));
}
