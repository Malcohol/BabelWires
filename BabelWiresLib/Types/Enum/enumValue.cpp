/**
 * Holds a single value of an enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

babelwires::EnumValue::EnumValue() = default;

babelwires::EnumValue::EnumValue(ShortId value)
    : IdentifierValueBase(value) {}

std::size_t babelwires::EnumValue::getHash() const {
    // eeee - Arbitrary discriminator.
    return hash::mixtureOf(0xeeee, get());
}

bool babelwires::EnumValue::operator==(const Value& other) const {
    const EnumValue* otherValue = other.tryAs<EnumValue>();
    return otherValue && (get() == otherValue->get());
}
