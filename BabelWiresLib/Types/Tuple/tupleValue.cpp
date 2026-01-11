/**
 * TupleValues are instances of TupleTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::TupleValue::TupleValue(Tuple componentValues)
    : m_componentValues(std::move(componentValues)) {}

unsigned int babelwires::TupleValue::getSize() const {
    return m_componentValues.size();
}

babelwires::ValueHolder& babelwires::TupleValue::getValue(unsigned int index) {
    assert((index < m_componentValues.size()) && "index out of range");
    return m_componentValues[index];
}

const babelwires::ValueHolder& babelwires::TupleValue::getValue(unsigned int index) const {
    assert((index < m_componentValues.size()) && "index out of range");
    return m_componentValues[index];
}

void babelwires::TupleValue::setValue(unsigned int index, ValueHolder newValue) {
    assert((index < m_componentValues.size()) && "index out of range");
    m_componentValues[index] = newValue;
}

std::size_t babelwires::TupleValue::getHash() const {
    std::size_t hash = hash::mixtureOf(m_componentValues.size());
    for (auto v : m_componentValues) {
        hash::mixInto(hash, v);
    }
    return hash;
}

bool babelwires::TupleValue::operator==(const Value& other) const {
    const TupleValue* const otherTuple = other.tryAs<TupleValue>();
    if (otherTuple == nullptr) {
        return false;
    }
    if (m_componentValues.size() != otherTuple->m_componentValues.size()) {
        return false;
    }
    for (int i = 0; i < m_componentValues.size(); ++i) {
        if (m_componentValues[i] != otherTuple->m_componentValues[i]) {
            return false;
        }
    }
    return true;
}

void babelwires::TupleValue::serializeContents(Serializer& serializer) const {
    std::vector<const EditableValue*> arrayToSerialize;
    for (const auto& v : m_componentValues) {
        arrayToSerialize.emplace_back(&v->getAsEditableValue());
    }
    serializer.serializeArray("componentValues", arrayToSerialize);
}

void babelwires::TupleValue::deserializeContents(Deserializer& deserializer) {
    auto typeIt = deserializer.deserializeArray<EditableValue>("componentValues", Deserializer::IsOptional::Optional);
    while (typeIt.isValid()) {
        m_componentValues.emplace_back(uniquePtrCast<Value>(typeIt.getObject()));
        ++typeIt;
    }
}

void babelwires::TupleValue::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& v : m_componentValues) {
        v.visitIdentifiers(visitor);
    }
}

void babelwires::TupleValue::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& v : m_componentValues) {
        v.visitFilePaths(visitor);
    }
}

bool babelwires::TupleValue::canContainIdentifiers() const {
    for (const auto& v : m_componentValues) {
        if (v->getAsEditableValue().canContainIdentifiers()) {
            return true;
        }
    }
    return false;
}

bool babelwires::TupleValue::canContainFilePaths() const {
    for (const auto& v : m_componentValues) {
        if (v->getAsEditableValue().canContainFilePaths()) {
            return true;
        }
    }
    return false;
}

std::string babelwires::TupleValue::toString() const {
    std::ostringstream os;
    os << "(";
    std::string sep = "";
    for (const auto& v : m_componentValues) {
        os << sep;
        sep = ", ";
        os << v->getAsEditableValue().toString();
    }
    os << ")";
    return os.str();
}
