/**
 * An ArrayValue can contain a dynamically-sized sequence of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Array/arrayValue.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::ArrayValue::ArrayValue(const TypeSystem& typeSystem, const Type& entryType, unsigned int initialSize)
{
    for (unsigned i = 0; i < initialSize; ++i) {
        insertValue(typeSystem, entryType, i);
    }
}

babelwires::ArrayValue::ArrayValue(const ArrayValue& other) = default;

babelwires::ArrayValue::ArrayValue(ArrayValue&& other) = default;

std::size_t babelwires::ArrayValue::getHash() const {
    std::size_t hash = hash::mixtureOf(m_values.size());
    for (unsigned int i = 0; i < m_values.size(); ++i) {
        hash::mixInto(hash, m_values[i]->getHash());
    }
    return hash;
}

bool babelwires::ArrayValue::operator==(const Value& other) const {
    const ArrayValue *const otherArray = other.as<ArrayValue>();
    if (otherArray == nullptr) {
        return false;
    }
    if (getSize() != otherArray->getSize()) {
        return false;
    }
    for (unsigned int i = 0; i < m_values.size(); ++i) {
        if (getValue(i) != otherArray->getValue(i)) {
            return false;
        }
    }
    return true;
}

unsigned int babelwires::ArrayValue::getSize() const {
    return m_values.size();
}

void babelwires::ArrayValue::setSize(const TypeSystem& typeSystem, const Type& entryType, unsigned int newSize) {
    if (newSize < m_values.size()) {
        m_values.resize(newSize);
    } else if (m_values.size() < newSize) {
        m_values.reserve(newSize);
        for (unsigned int i = m_values.size(); i < newSize; ++i) {
            insertValue(typeSystem, entryType, i);
        }
    }
}

const babelwires::ValueHolder& babelwires::ArrayValue::getValue(unsigned int index) const {
    assert(index < m_values.size());
    return m_values[index];
}

babelwires::ValueHolder& babelwires::ArrayValue::getValue(unsigned int index) {
    assert(index < m_values.size());
    return m_values[index];
}

void babelwires::ArrayValue::setValue(unsigned int index, ValueHolder newValue) {
    assert(index < m_values.size());
    m_values[index] = std::move(newValue);
}

void babelwires::ArrayValue::insertValue(const TypeSystem& typeSystem, const Type& entryType, unsigned int index) {
    assert(index <= m_values.size());
    m_values.insert(m_values.begin() + index, entryType.createValue(typeSystem));
}

void babelwires::ArrayValue::removeValue(unsigned int index) {
    assert(index < m_values.size());
    m_values.erase(m_values.begin() + index);
}
