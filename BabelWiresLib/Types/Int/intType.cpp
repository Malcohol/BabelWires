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
}

babelwires::NewValueHolder babelwires::IntType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<IntValue>(m_defaultValue);
}

bool babelwires::IntType::isValidValue(const Value& v) const {
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

babelwires::DefaultIntType::DefaultIntType() : IntType() {}
