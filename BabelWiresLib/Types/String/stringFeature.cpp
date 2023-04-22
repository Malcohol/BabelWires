/**
 * A feature which manages a single StringValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/String/stringFeature.hpp>

#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

babelwires::StringFeature::StringFeature()
    : SimpleValueFeature(StringType::getThisIdentifier()) {

    }

std::string babelwires::StringFeature::get() const {
    return getValue().is<StringValue>().get();
}

void babelwires::StringFeature::set(std::string value) {
    setValue(StringValue(value));
}
