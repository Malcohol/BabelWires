/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <BabelWiresLib/Types/Enum/enumValue.hpp>

babelwires::EnumType::EnumType(TypeExp&& typeExp, ValueSet values, unsigned int indexOfDefaultValue)
    : Type(std::move(typeExp))
    , m_values(std::move(values))
    , m_indexOfDefaultValue(indexOfDefaultValue) {
    m_sortedValues = m_values;
    std::sort(m_sortedValues.begin(), m_sortedValues.end());
    m_valueToIndex.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        assert((m_values[i].getDiscriminator() != 0) && "Only registered ids can be used in an enum");
        assert((m_valueToIndex.find(m_values[i]) == m_valueToIndex.end()) && "Enum has duplicate value");
        m_valueToIndex.insert({m_values[i], i});
    }
}

const babelwires::EnumType::ValueSet& babelwires::EnumType::getValueSet() const {
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
    const ValueSet& values = getValueSet();
    const auto it = m_valueToIndex.find(id);
    assert((it != m_valueToIndex.end()) && "id not found in enum");
    return it->second;
}

babelwires::ShortId babelwires::EnumType::getIdentifierFromIndex(unsigned int index) const {
    const ValueSet& values = getValueSet();
    assert(index < values.size());
    return values[index];
}

bool babelwires::EnumType::isAValue(const babelwires::ShortId& id) const {
    const ValueSet& values = getValueSet();
    const auto it = m_valueToIndex.find(id);
    if (it == m_valueToIndex.end()) {
        return false;
    }
    // TODO DISCRIMINATORS
    return true;
}

bool babelwires::EnumType::visitValue(const TypeSystem& typeSystem, const Value& v,
                                ChildValueVisitor& visitor) const {
    if (const auto* enumValue = v.as<EnumValue>()) {
        return isAValue(enumValue->get());
    }
    return false;
}

babelwires::NewValueHolder babelwires::EnumType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<EnumValue>(getIdentifierFromIndex(getIndexOfDefaultValue()));
}

std::string babelwires::EnumType::getFlavour() const {
    return EnumValue::serializationType;
}

std::optional<babelwires::SubtypeOrder> babelwires::EnumType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                    const Type& other) const {
    const EnumType* const otherEnum = other.as<EnumType>();
    if (!otherEnum) {
        return {};
    }
    auto it = m_sortedValues.begin();
    auto otherIt = otherEnum->m_sortedValues.begin();
    bool foundValueNotInOther = false;
    bool foundValueNotInThis = false;
    bool foundValueInBoth = false;
    while ((it < m_sortedValues.end()) && (otherIt < otherEnum->m_sortedValues.end())) {
        if (*it < *otherIt) {
            foundValueNotInOther = true;
            if (foundValueNotInThis && foundValueInBoth) {
                return SubtypeOrder::IsIntersecting;
            }
            ++it;
        } else if (*otherIt < *it) {
            foundValueNotInThis = true;
            if (foundValueNotInOther && foundValueInBoth) {
                return SubtypeOrder::IsIntersecting;
            }
            ++otherIt;
        } else {
            foundValueInBoth = true;
            if (foundValueNotInOther && foundValueNotInThis) {
                return SubtypeOrder::IsIntersecting;
            }
            ++it;
            ++otherIt;
        }
    }
    if (it != m_sortedValues.end()) {
        foundValueNotInOther = true;
    } else if (otherIt != otherEnum->m_sortedValues.end()) {
        foundValueNotInThis = true;
    }
    if (foundValueNotInOther && foundValueNotInThis) {
        return foundValueInBoth ? SubtypeOrder::IsIntersecting : SubtypeOrder::IsDisjoint;
    } else if (foundValueNotInOther) {
        return SubtypeOrder::IsSupertype;
    } else if (foundValueNotInThis) {
        return SubtypeOrder::IsSubtype;
    } else {
        return SubtypeOrder::IsEquivalent;
    }
}

std::string babelwires::EnumType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<EnumValue>().toString();
}
