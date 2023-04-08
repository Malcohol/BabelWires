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

babelwires::IntFeature::IntFeature()
    : SimpleValueFeature(DefaultIntType::getThisIdentifier()) {}

babelwires::IntFeature::IntFeature(IntValue::NativeType defaultValue)
    : SimpleValueFeature(TypeRef(IntTypeConstructor::getThisIdentifier(),
                                 IntValue(std::numeric_limits<IntValue::NativeType>::min()),
                                 IntValue(std::numeric_limits<IntValue::NativeType>::max()), IntValue(defaultValue))) {}

babelwires::IntFeature::IntFeature(Range<IntValue::NativeType> range, IntValue::NativeType defaultValue)
    : SimpleValueFeature(TypeRef(IntTypeConstructor::getThisIdentifier(), IntValue(range.m_min), IntValue(range.m_max),
                                 IntValue(range.clamp(defaultValue)))) {}

babelwires::IntValue::NativeType babelwires::IntFeature::get() const {
    return getValue().is<IntValue>().get();
}

void babelwires::IntFeature::set(IntValue::NativeType value) {
    setValue(IntValue(value));
}
