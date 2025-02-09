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

babelwires::TupleValue::TupleValue(Tuple values)
    : m_values(std::move(values)) {}

unsigned int babelwires::TupleValue::getSize() const {
    return m_values.size();
}

babelwires::EditableValueHolder& babelwires::TupleValue::getValue(unsigned int index) {
    assert((index < m_values.size()) && "index out of range");
    return m_values[index];
}

const babelwires::EditableValueHolder& babelwires::TupleValue::getValue(unsigned int index) const {
    assert((index < m_values.size()) && "index out of range");
    return m_values[index];
}

void babelwires::TupleValue::setValue(unsigned int index, EditableValueHolder newValue) {
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

void babelwires::TupleValue::serializeContents(Serializer& serializer) const {
    std::vector<const EditableValue*> arrayToSerialize;
    for (const auto& v : m_values) {
        arrayToSerialize.emplace_back(&*v);
    }
    serializer.serializeArray("values", arrayToSerialize);
}

void babelwires::TupleValue::deserializeContents(Deserializer& deserializer) {
    auto typeIt = deserializer.deserializeArray<EditableValue>("values", Deserializer::IsOptional::Optional);
    while (typeIt.isValid()) {
        m_values.emplace_back(std::move(*typeIt.getObject()));
        ++typeIt;
    }
}

void babelwires::TupleValue::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& v : m_values) {
        v.visitIdentifiers(visitor);
    }
}

void babelwires::TupleValue::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& v : m_values) {
        v.visitFilePaths(visitor);
    }
}

bool babelwires::TupleValue::canContainIdentifiers() const {
    for (const auto& v : m_values) {
        if (v->canContainIdentifiers()) {
            return true;
        }
    }
    return false;
}

bool babelwires::TupleValue::canContainFilePaths() const {
    for (const auto& v : m_values) {
        if (v->canContainFilePaths()) {
            return true;
        }
    }
    return false;
}

std::string babelwires::TupleValue::toString() const {
    std::ostringstream os;
    os << "(";
    std::string sep = "";
    for (const auto& v : m_values) {
        os << sep;
        sep = ", ";
        os << v->toString();
    }
    os << ")";
    return os.str();
}
