/**
 * Holds the identifier of a field.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/fieldIdValue.hpp>

babelwires::FieldIdValue::FieldIdValue() = default;

babelwires::FieldIdValue::FieldIdValue(ShortId value)
    : IdentifierValueBase(value) {}

std::size_t babelwires::FieldIdValue::getHash() const {
    // ffff - Arbitrary discriminator.
    return hash::mixtureOf(0xffff, get());
}

bool babelwires::FieldIdValue::operator==(const Value& other) const {
    const FieldIdValue* otherValue = other.as<FieldIdValue>();
    return otherValue && (get() == otherValue->get());
}
