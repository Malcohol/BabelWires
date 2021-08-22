/**
 * An interface for objects which associate values and human-friendly names.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/ValueNames/valueNames.hpp"

#include <cassert>

void babelwires::ValueNames::NamedValueIterator::operator++() {
    assert(!m_atEnd && "Iterator is invalid");
    if (!m_valueNamesContainer.getNextValueWithName(m_value)) {
        m_atEnd = true;
    }
}

bool babelwires::ValueNames::NamedValueIterator::operator==(const NamedValueIterator& other) const {
    assert((&m_valueNamesContainer == &other.m_valueNamesContainer) && "Comparing iterators of different int features");
    return (m_atEnd == other.m_atEnd) && (m_atEnd || (m_value == other.m_value));
}

bool babelwires::ValueNames::NamedValueIterator::operator!=(const NamedValueIterator& other) const {
    assert((&m_valueNamesContainer == &other.m_valueNamesContainer) && "Comparing iterators of different int features");
    return !(*this == other);
}

std::tuple<int, std::string> babelwires::ValueNames::NamedValueIterator::operator*() const {
    assert(!m_atEnd && "Iterator is invalid");
    std::string name;
    const bool result = m_valueNamesContainer.getNameForValue(m_value, name);
    assert(result && "Iterator is invalid");
    return std::tuple<int, std::string>(m_value, name);
}

babelwires::ValueNames::NamedValueIterator babelwires::ValueNames::begin() const {
    return NamedValueIterator{getFirstValue(), false, *this};
}

babelwires::ValueNames::NamedValueIterator babelwires::ValueNames::end() const {
    return NamedValueIterator{0, true, *this};
}

bool babelwires::ValueNames::getValueForName(const std::string& name, int& valueOut) const {
    return doGetValueForName(name, valueOut);
}

bool babelwires::ValueNames::getNameForValue(int value, std::string& nameOut) const {
    // assert(getRange().contains(value) && "value is not in the range");
    return doGetNameForValue(value, nameOut);
}
