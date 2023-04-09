/**
 * The type of integers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

babelwires::IntFeature2::IntFeature2()
    : SimpleValueFeature(DefaultIntType::getThisIdentifier()) {}

babelwires::IntFeature2::IntFeature2(IntValue::NativeType defaultValue)
    : SimpleValueFeature(TypeRef(IntTypeConstructor::getThisIdentifier(),
                                 IntValue(std::numeric_limits<IntValue::NativeType>::min()),
                                 IntValue(std::numeric_limits<IntValue::NativeType>::max()), IntValue(defaultValue))) {}

babelwires::IntFeature2::IntFeature2(IntValue::NativeType minValue, IntValue::NativeType maxValue, IntValue::NativeType defaultValue)
    : SimpleValueFeature(TypeRef(IntTypeConstructor::getThisIdentifier(), IntValue(minValue), IntValue(maxValue),
                                 IntValue(std::clamp(defaultValue, minValue, maxValue)))) {}

babelwires::IntValue::NativeType babelwires::IntFeature2::get() const {
    return getValue().is<IntValue>().get();
}

void babelwires::IntFeature2::set(IntValue::NativeType value) {
    setValue(IntValue(value));
}
