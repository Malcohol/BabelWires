/**
 * ListValue stores a sequence of elements of the same type.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/List/listValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ListValue::ListValue() = default;

babelwires::ListValue::ListValue(const ListValue& other)
    : m_elementTypeRef(other.m_elementTypeRef) {
    for (const auto& element : other.m_elements) {
        m_elements.push_back(element->clone());
    }
}

babelwires::ListValue::ListValue(ListValue&& other)
    : m_elementTypeRef(std::move(other.m_elementTypeRef))
    , m_elements(std::move(other.m_elements)) {}

babelwires::ListValue::ListValue(const TypeSystem& typeSystem, TypeRef elementRef)
    : m_elementTypeRef(std::move(elementRef)) {}

babelwires::ListValue& babelwires::ListValue::operator=(const ListValue& other) {
    m_elementTypeRef = other.m_elementTypeRef;
    m_elements.clear();
    for (const auto& element : other.m_elements) {
        m_elements.push_back(element->clone());
    }
    return *this;
}

babelwires::ListValue& babelwires::ListValue::operator=(ListValue&& other) {
    m_elementTypeRef = std::move(other.m_elementTypeRef);
    m_elements = std::move(other.m_elements);
    return *this;
}

const babelwires::TypeRef& babelwires::ListValue::getElementTypeRef() const {
    return m_elementTypeRef;
}

void babelwires::ListValue::setElementTypeRef(const TypeRef& elementRef) {
    m_elementTypeRef = elementRef;
}

unsigned int babelwires::ListValue::getNumElements() const {
    return m_elements.size();
}

const babelwires::EditableValue& babelwires::ListValue::getElement(unsigned int index) const {
    return *m_elements[index];
}

void babelwires::ListValue::setElement(unsigned int index, std::unique_ptr<EditableValue> newElement) {
    assert(index < m_elements.size() && "Index out of bounds");
    m_elements[index] = std::move(newElement);
}

void babelwires::ListValue::clear() {
    m_elements.clear();
}

void babelwires::ListValue::pushBack(std::unique_ptr<EditableValue> newElement) {
    m_elements.push_back(std::move(newElement));
}

bool babelwires::ListValue::operator==(const ListValue& other) const {
    if (m_elementTypeRef != other.m_elementTypeRef) {
        return false;
    }
    if (m_elements.size() != other.m_elements.size()) {
        return false;
    }
    for (size_t i = 0; i < m_elements.size(); ++i) {
        if (*m_elements[i] != *other.m_elements[i]) {
            return false;
        }
    }
    return true;
}

bool babelwires::ListValue::operator==(const Value& other) const {
    if (const ListValue* otherList = other.as<ListValue>()) {
        return *this == *otherList;
    }
    return false;
}

std::size_t babelwires::ListValue::getHash() const {
    std::size_t hash = m_elementTypeRef.getHash();
    for (const auto& element : m_elements) {
        hash::mixInto(hash, element->getHash());
    }
    return hash;
}

std::string babelwires::ListValue::toString() const {
    std::string result = "[";
    for (size_t i = 0; i < m_elements.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += m_elements[i]->toString();
    }
    result += "]";
    return result;
}

bool babelwires::ListValue::canContainIdentifiers() const {
    return true;
}

bool babelwires::ListValue::canContainFilePaths() const {
    return true;
}

void babelwires::ListValue::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(m_elementTypeRef, "elementType");
    serializer.serializeArray("elements", m_elements);
}

void babelwires::ListValue::deserializeContents(Deserializer& deserializer) {
    m_elementTypeRef = std::move(*deserializer.deserializeObject<TypeRef>("elementType"));
    m_elements.clear();
    auto it = deserializer.deserializeArray<EditableValue>("elements", Deserializer::IsOptional::Optional);
    while (it.isValid()) {
        std::unique_ptr<EditableValue> newEntry = it.getObject();
        m_elements.emplace_back(std::move(newEntry));
        ++it;
    }
}

void babelwires::ListValue::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& element : m_elements) {
        element->visitIdentifiers(visitor);
    }
}

void babelwires::ListValue::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& element : m_elements) {
        element->visitFilePaths(visitor);
    }
}

bool babelwires::ListValue::isValid(const TypeSystem& typeSystem) const {
    const Type* elementType = m_elementTypeRef.tryResolve(typeSystem);
    if (!elementType) {
        return false;
    }

    for (const auto& element : m_elements) {
        if (!elementType->isValidValue(typeSystem, *element)) {
            return false;
        }
    }
    return true;
}