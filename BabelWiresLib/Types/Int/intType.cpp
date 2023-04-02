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

babelwires::NewValueHolder babelwires::IntType::createValue() const {
    return ValueHolder::makeValue<IntValue>();
}

bool babelwires::IntType::isValidValue(const Value& v) const {
    return v.as<IntValue>();
}
