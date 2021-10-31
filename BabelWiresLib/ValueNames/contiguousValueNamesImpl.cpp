/**
 * Implementations for ValueNames.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/ValueNames/contiguousValueNamesImpl.hpp"

#include <cassert>

namespace {
    babelwires::ContiguousValueNamesImpl::ValueFromNamesMap
    initializeValueFromNamesMap(const babelwires::ContiguousValueNamesImpl::Names& names, int offset) {
        babelwires::ContiguousValueNamesImpl::ValueFromNamesMap valueFromNames;
        for (int i = 0; i < names.size(); ++i) {
            // TODO assert uniqueness in both directions.
            valueFromNames.insert(std::make_pair(names[i], i + offset));
        }
        return valueFromNames;
    }
} // namespace

babelwires::ContiguousValueNamesImpl::ContiguousValueNamesImpl(Names names, int offset)
    : m_names(std::move(names))
    , m_valueFromNames(initializeValueFromNamesMap(m_names, offset))
    , m_offset(offset) {
    assert((m_names.size() > 0) && "You can't construct a ContiguousValueNamesImpl with no value name pairs");
}

int babelwires::ContiguousValueNamesImpl::getFirstValue() const {
    return m_offset;
}

bool babelwires::ContiguousValueNamesImpl::getNextValueWithName(int& value) const {
    if (value - m_offset < m_names.size() - 1) {
        ++value;
        return true;
    } else {
        return false;
    }
}

bool babelwires::ContiguousValueNamesImpl::doGetValueForName(const std::string& name, int& valueOut) const {
    const auto it = m_valueFromNames.find(name);
    if (it != m_valueFromNames.end()) {
        valueOut = it->second;
        return true;
    } else {
        return false;
    }
}

bool babelwires::ContiguousValueNamesImpl::doGetNameForValue(int value, std::string& nameOut) const {
    const int index = value - m_offset;
    if ((index >= 0) && (index < m_names.size())) {
        nameOut = m_names[index];
        return true;
    } else {
        return false;
    }
}
