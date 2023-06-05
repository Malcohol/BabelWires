/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Rational/rationalType.hpp>

#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::RationalType::RationalType(Range<Rational> range, Rational defaultValue) 
    : m_range(range)
    , m_defaultValue(defaultValue)
{
}

babelwires::NewValueHolder babelwires::RationalType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<RationalValue>(m_defaultValue);
}

bool babelwires::RationalType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const RationalValue* const ratValue = v.as<RationalValue>()) {
        return m_range.contains(ratValue->get());
    }
    return false;
}

babelwires::Range<babelwires::Rational> babelwires::RationalType::getRange() const {
    return m_range;
}

std::string babelwires::RationalType::getKind() const {
    return RationalValue::serializationType;
}

babelwires::DefaultRationalType::DefaultRationalType() : RationalType() {}
