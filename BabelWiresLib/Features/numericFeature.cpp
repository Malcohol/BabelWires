/**
 * ValueFeatures carrying numeric values, e.g. Integers and Rationals.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/numericFeature.hpp"

const babelwires::ValueNames* babelwires::IntFeature::getValueNames() const {
    return nullptr;
}

std::string babelwires::IntFeature::doGetValueType() const {
    return "int";
}

std::string babelwires::RationalFeature::doGetValueType() const {
    return "rat";
}
