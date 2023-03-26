/**
 * A StringFeature is a ValueFeature which carries a string.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/stringFeature.hpp>

#include <BabelWiresLib/TypeSystem/stringType.hpp>
#include <BabelWiresLib/TypeSystem/stringValue.hpp>

babelwires::StringFeature::StringFeature()
    : SimpleValueFeature(StringType::getThisIdentifier()) {

    }

std::string babelwires::StringFeature::get() const {
    return getValue().is<StringValue>().get();
}

void babelwires::StringFeature::set(std::string value) {
    setValue(StringValue(value));
}
