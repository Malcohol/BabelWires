/**
 * TupleValues are instances of TupleTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

babelwires::TupleValue::TupleValue(Tuple values)
    : m_values(std::move(values)) {}

unsigned int babelwires::TupleValue::getSize() const {
    return m_values.size();
}

babelwires::ValueHolder& babelwires::TupleValue::getValue(unsigned int index) {
    assert((index < m_values.size()) && "index out of range");
    return m_values[index];
}

const babelwires::ValueHolder& babelwires::TupleValue::getValue(unsigned int index) const {
    assert((index < m_values.size()) && "index out of range");
    return m_values[index];
}

void babelwires::TupleValue::setValue(unsigned int index, ValueHolder newValue) {
    assert((index < m_values.size()) && "index out of range");
    m_values[index] = newValue;
}

std::size_t babelwires::TupleValue::getHash() const {
    std::size_t hash = hash::mixtureOf(m_values.size());
    for (auto v : m_values) {
        hash::mixInto(hash, v);
    }
    return hash;
}

bool babelwires::TupleValue::operator==(const Value& other) const {
    const TupleValue* const otherTuple = other.as<TupleValue>();
    if (otherTuple == nullptr) {
        return false;
    }
    if (m_values.size() != otherTuple->m_values.size()) {
        return false;
    }
    for (int i = 0; i < m_values.size(); ++i) {
        if (m_values[i] != otherTuple->m_values[i]) {
            return false;
        }
    }
    return true;
}
