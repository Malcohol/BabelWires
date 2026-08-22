/**
 * The type of fixed-point values.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Fixed/fixedType.hpp>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <BaseLib/Identifiers/registeredIdentifier.hpp>

babelwires::FixedType::FixedType(TypeExp&& typeExpOfThis, int precision, Range<Fixed::NativeType> range,
                                Fixed::NativeType defaultValue)
    : Type(std::move(typeExpOfThis))
    , m_defaultValue(defaultValue)
    , m_range(range)
    , m_precision(precision) {
    assert(m_precision >= 0);
    assert(m_range.contains(defaultValue));
}

babelwires::NewValueHolder babelwires::FixedType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<FixedValue>(Fixed(m_defaultValue, m_precision));
}

bool babelwires::FixedType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    if (const auto* const value = v.tryAs<FixedValue>()) {
        return (value->get().getPrecision() == m_precision) && m_range.contains(value->get().getNumerator());
    }
    return false;
}

int babelwires::FixedType::getPrecision() const {
    return m_precision;
}

babelwires::Range<babelwires::Fixed::NativeType> babelwires::FixedType::getRange() const {
    return m_range;
}

std::string babelwires::FixedType::getFlavour() const {
    return FixedValue::s_serializationTypeName;
}

std::optional<babelwires::SubtypeOrder> babelwires::FixedType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                  const Type& other) const {
    const auto* const otherFixedType = other.tryAs<FixedType>();
    if (!otherFixedType) {
        return {};
    }
    if (otherFixedType->getPrecision() != m_precision) {
        return SubtypeOrder::IsDisjoint;
    }
    return subtypeFromRanges(getRange(), otherFixedType->getRange());
}

std::string babelwires::FixedType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    return v->as<FixedValue>().toString();
}

babelwires::DefaultFixedType::DefaultFixedType()
    : FixedType(getThisIdentifier(), s_defaultPrecision) {}
