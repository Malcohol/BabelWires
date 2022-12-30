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
                       unsigned int indexOfDefaultValue)
    : Type(identifier, version)
    , m_values(std::move(values))
    , m_indexOfDefaultValue(indexOfDefaultValue) {
    m_valueToIndex.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        assert((m_values[i].getDiscriminator() != 0) && "Only registered ids can be used in an enum");
        m_valueToIndex.insert({m_values[i], i});
    }
}

const babelwires::Enum::EnumValues& babelwires::Enum::getEnumValues() const {
    return m_values;
}

unsigned int babelwires::Enum::getIndexOfDefaultValue() const {
    return m_indexOfDefaultValue;
}

int babelwires::Enum::tryGetIndexFromIdentifier(babelwires::Identifier id) const {
    const auto it = m_valueToIndex.find(id);
    if(it != m_valueToIndex.end()) {
        return it->second;
    }
    return -1;
}

unsigned int babelwires::Enum::getIndexFromIdentifier(babelwires::Identifier id) const {
    const EnumValues& values = getEnumValues();
    const auto it = m_valueToIndex.find(id);
    assert((it != m_valueToIndex.end()) && "id not found in enum");
    return it->second;
}

babelwires::Identifier babelwires::Enum::getIdentifierFromIndex(unsigned int index) const {
    const EnumValues& values = getEnumValues();
    assert(index < values.size());
    return values[index];
}

bool babelwires::Enum::isAValue(const babelwires::Identifier& id) const {
    const EnumValues& values = getEnumValues();
    const auto it = m_valueToIndex.find(id);
    if (it == m_valueToIndex.end()) {
        return false;
    }
    // TODO Needed?
    id.setDiscriminator(it->first.getDiscriminator());
    return true;
}

std::unique_ptr<babelwires::Value> babelwires::Enum::createValue() const {
    return std::make_unique<EnumValue>(getIdentifierFromIndex(getIndexOfDefaultValue()));
}

bool babelwires::Enum::verifySupertype(const Type& supertype) const {
    const Enum& parentEnum = supertype.is<Enum>();
    for (const auto& v : m_values) {
        assert(parentEnum.isAValue(v));
    }
    return true;
}
