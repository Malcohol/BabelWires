/**
 * The FailureType has this as its only value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Failure/failureValue.hpp>

std::size_t babelwires::FailureValue::getHash() const { 
    return 0xFA17; // FAIL
}

bool babelwires::FailureValue::operator==(const Value& other) const {
    return other.as<FailureValue>();
}
