/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Enums/enum.hpp>

#include <BabelWiresLib/TypeSystem/enumValue.hpp>

babelwires::Enum::Enum(LongIdentifier identifier, VersionNumber version, EnumValues values,
                       unsigned int indexOfDefaultValue, std::optional<LongIdentifier> parentTypeId)
    : Type(identifier, version, parentTypeId)
    , m_values(std::move(values))
    , m_indexOfDefaultValue(indexOfDefaultValue) {
#ifndef NDEBUG
    if (!parentTypeId) {
        /// For enums with parents, their discriminators will be resolved in verifyParent.
        for (int i = 0; i < values.size(); ++i) {
            assert((values[i].getDiscriminator() != 0) && "Only registered ids can be used in an enum");
        }
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
    return static_cast<unsigned int>(it - values.begin());
}

babelwires::Identifier babelwires::Enum::getIdentifierFromIndex(unsigned int index) const {
    const EnumValues& values = getEnumValues();
    assert(index < values.size());
    return values[index];
}

bool babelwires::Enum::isAValue(const babelwires::Identifier& id) const {
    const EnumValues& values = getEnumValues();
    const auto it = std::find(values.begin(), values.end(), id);
    if (it == values.end()) {
        return false;
    }
    id.setDiscriminator(it->getDiscriminator());
    return true;
}

std::unique_ptr<babelwires::Value> babelwires::Enum::createValue() const {
    return std::make_unique<EnumValue>(getIdentifierFromIndex(getIndexOfDefaultValue()));
}

bool babelwires::Enum::verifyParent(const Type& parentType) const {
    const Enum& parentEnum = parentType.is<Enum>();
    for (const auto& v : m_values) {
        assert(parentEnum.isAValue(v));
    }
    return true;
}
