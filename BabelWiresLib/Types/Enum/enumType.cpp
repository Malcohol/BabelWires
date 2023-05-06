/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <BabelWiresLib/Types/Enum/enumValue.hpp>

babelwires::EnumType::EnumType(EnumValues values, unsigned int indexOfDefaultValue)
    : m_values(std::move(values))
    , m_indexOfDefaultValue(indexOfDefaultValue) {
    m_valueToIndex.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        assert((m_values[i].getDiscriminator() != 0) && "Only registered ids can be used in an enum");
        assert((m_valueToIndex.find(m_values[i]) == m_valueToIndex.end()) && "Enum has duplicate value");
        m_valueToIndex.insert({m_values[i], i});
    }
}

const babelwires::EnumType::EnumValues& babelwires::EnumType::getEnumValues() const {
    return m_values;
}

unsigned int babelwires::EnumType::getIndexOfDefaultValue() const {
    return m_indexOfDefaultValue;
}

int babelwires::EnumType::tryGetIndexFromIdentifier(babelwires::ShortId id) const {
    const auto it = m_valueToIndex.find(id);
    if (it != m_valueToIndex.end()) {
        return it->second;
    }
    return -1;
}

unsigned int babelwires::EnumType::getIndexFromIdentifier(babelwires::ShortId id) const {
    const EnumValues& values = getEnumValues();
    const auto it = m_valueToIndex.find(id);
    assert((it != m_valueToIndex.end()) && "id not found in enum");
    return it->second;
}

babelwires::ShortId babelwires::EnumType::getIdentifierFromIndex(unsigned int index) const {
    const EnumValues& values = getEnumValues();
    assert(index < values.size());
    return values[index];
}

bool babelwires::EnumType::isAValue(const babelwires::ShortId& id) const {
    const EnumValues& values = getEnumValues();
    const auto it = m_valueToIndex.find(id);
    if (it == m_valueToIndex.end()) {
        return false;
    }
    // TODO Needed?
    id.setDiscriminator(it->first.getDiscriminator());
    return true;
}

bool babelwires::EnumType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const auto* enumValue = v.as<EnumValue>()) {
        return isAValue(enumValue->get());
    }
    return false;
}

babelwires::NewValueHolder babelwires::EnumType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<EnumValue>(getIdentifierFromIndex(getIndexOfDefaultValue()));
}

bool babelwires::EnumType::verifySupertype(const Type& supertype) const {
    const EnumType& parentEnum = supertype.is<EnumType>();
    for (const auto& v : m_values) {
        ShortId parentId = v;
        assert(parentEnum.isAValue(parentId));
        // I don't _know_ that this would cause problems, but it simplifies things to require this.
        assert((parentId.getDiscriminator() == v.getDiscriminator()) &&
               "An enum cannot be related to another enum with a different value with the same identifier");
    }
    return true;
}

std::string babelwires::EnumType::getKind() const {
    return EnumValue::serializationType;
}
