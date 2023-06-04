/**
 * A feature which manages a single RationalValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Rational/rationalFeature.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>

babelwires::RationalFeature::RationalFeature()
    : SimpleValueFeature(DefaultRationalType::getThisIdentifier()) {}

babelwires::RationalFeature::RationalFeature(Rational defaultValue)
    : SimpleValueFeature(TypeRef(RationalTypeConstructor::getThisIdentifier(),
                                 RationalValue(std::numeric_limits<Rational>::min()),
                                 RationalValue(std::numeric_limits<Rational>::max()), RationalValue(defaultValue))) {}

babelwires::RationalFeature::RationalFeature(Rational minValue, Rational maxValue, Rational defaultValue)
    : SimpleValueFeature(TypeRef(RationalTypeConstructor::getThisIdentifier(), RationalValue(minValue), RationalValue(maxValue),
                                 RationalValue(std::clamp(defaultValue, minValue, maxValue)))) {}

babelwires::Rational babelwires::RationalFeature::get() const {
    return getValue()->is<RationalValue>().get();
}

void babelwires::RationalFeature::set(Rational value) {
    setValue(RationalValue(value));
}
