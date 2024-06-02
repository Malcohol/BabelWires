/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intType.hpp>

#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::IntType::IntType(Range<IntValue::NativeType> range, IntValue::NativeType defaultValue) 
    : m_range(range)
    , m_defaultValue(defaultValue)
{
    assert(m_range.contains(defaultValue));
}

babelwires::NewValueHolder babelwires::IntType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<IntValue>(m_defaultValue);
}

bool babelwires::IntType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const IntValue* const intValue = v.as<IntValue>()) {
        return m_range.contains(intValue->get());
    }
    return false;
}

babelwires::Range<babelwires::IntValue::NativeType> babelwires::IntType::getRange() const {
    return m_range;
}

std::string babelwires::IntType::getKind() const {
    return IntValue::serializationType;
}

babelwires::SubtypeOrder babelwires::IntType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const Type& other) const {
    const IntType *const otherIntType = other.as<IntType>();
    if (!otherIntType) {
        return SubtypeOrder::IsUnrelated;
    }

    const auto& rangeThis = getRange();
    const auto& rangeOther = otherIntType->getRange();

    const bool thisSubOther = rangeOther.contains(rangeThis);
    const bool otherSubThis = rangeThis.contains(rangeOther);

    if (thisSubOther && otherSubThis) {
        return SubtypeOrder::IsEquivalent;
    } else if (thisSubOther) {
        return SubtypeOrder::IsSubtype;
    } else if (otherSubThis) {
        return SubtypeOrder::IsSupertype;
    } else {
        return SubtypeOrder::IsUnrelated;
    }

    return SubtypeOrder::IsEquivalent;
}

std::string babelwires::IntType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<IntValue>().toString();
}

babelwires::DefaultIntType::DefaultIntType() : IntType() {}
