/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Rational/rationalType.hpp>

#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::RationalType::RationalType(TypeExp&& typeExp, Range<Rational> range, Rational defaultValue) 
    : Type(std::move(typeExp))
    , m_range(range)
    , m_defaultValue(defaultValue)
{
}

babelwires::NewValueHolder babelwires::RationalType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<RationalValue>(m_defaultValue);
}

bool babelwires::RationalType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    if (const RationalValue* const ratValue = v.as<RationalValue>()) {
        return m_range.contains(ratValue->get());
    }
    return false;
}

babelwires::Range<babelwires::Rational> babelwires::RationalType::getRange() const {
    return m_range;
}

std::string babelwires::RationalType::getFlavour() const {
    return RationalValue::serializationType;
}

std::optional<babelwires::SubtypeOrder> babelwires::RationalType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const Type& other) const {
    const RationalType *const otherRationalType = other.as<RationalType>();
    if (!otherRationalType) {
        return {};
    }
    return subtypeFromRanges(getRange(), otherRationalType->getRange());
}

std::string babelwires::RationalType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<RationalValue>().toString();
}

babelwires::DefaultRationalType::DefaultRationalType() : RationalType(getThisType()) {}

