/**
 * Implementations for ValueNames.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/ValueNames/sparseValueNamesImpl.hpp"

#include <cassert>

namespace {
    babelwires::SparseValueNamesImpl::ValueFromNamesMap
    initializeValueFromNamesMap(const babelwires::SparseValueNamesImpl::NameFromValuesMap& nameFromValues) {
        babelwires::SparseValueNamesImpl::ValueFromNamesMap valueFromNames;
        for (auto pair : nameFromValues) {
            // TODO assert uniqueness in both directions.
            valueFromNames.insert(std::make_pair(pair.second, pair.first));
        }
        return valueFromNames;
    }
} // namespace

babelwires::SparseValueNamesImpl::SparseValueNamesImpl(const NameFromValuesMap& nameFromValues)
    : m_nameFromValues(nameFromValues)
    , m_valueFromNames(initializeValueFromNamesMap(nameFromValues)) {
    assert((nameFromValues.size() > 0) && "You can't construct a SparseValueNamesImpl with no value name pairs");
}

int babelwires::SparseValueNamesImpl::getFirstValue() const {
    return m_nameFromValues.begin()->first;
}

bool babelwires::SparseValueNamesImpl::getNextValueWithName(int& value) const {
    auto it = m_nameFromValues.find(value);
    assert((it != m_nameFromValues.end()) && "Value did not have a name");
    ++it;
    if (it == m_nameFromValues.end()) {
        return false;
    } else {
        value = it->first;
        return true;
    }
}

bool babelwires::SparseValueNamesImpl::doGetValueForName(const std::string& name, int& valueOut) const {
    const auto it = m_valueFromNames.find(name);
    if (it == m_valueFromNames.end()) {
        return false;
    } else {
        valueOut = it->second;
        return true;
    }
}

bool babelwires::SparseValueNamesImpl::doGetNameForValue(int value, std::string& nameOut) const {
    const auto it = m_nameFromValues.find(value);
    if (it == m_nameFromValues.end()) {
        return false;
    } else {
        nameOut = it->second;
        return true;
    }
}
