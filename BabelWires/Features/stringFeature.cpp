/**
 * A StringFeature is a ValueFeature which carries a string.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Features/stringFeature.hpp"

std::string babelwires::StringFeature::get() const {
    return m_value;
}

void babelwires::StringFeature::set(std::string value) {
    if (value != m_value) {
        m_value = std::move(value);
        setChanged(Changes::ValueChanged);
    }
}

void babelwires::StringFeature::doSetToDefault() {
    set("");
}

std::string babelwires::StringFeature::doGetValueType() const {
    return "str";
}

void babelwires::StringFeature::doAssign(const ValueFeature& other) {
    const auto& otherString = *other.asA<StringFeature>();
    set(otherString.get());
}

std::size_t babelwires::StringFeature::doGetHash() const {
    return std::hash<std::string>{}(m_value);
}
