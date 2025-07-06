/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericValue.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::TypeRef babelwires::GenericValue::getTypeRef() const {
    return m_typeRef;
}

const babelwires::ValueHolder& babelwires::GenericValue::getValue() const {
    return m_value;
}

babelwires::ValueHolder& babelwires::GenericValue::getValue() {
    return m_value;
}

void babelwires::GenericValue::setTypeRef(const TypeSystem& typeSystem, TypeRef typeRef) {
    if (m_typeRef != typeRef) {
        m_typeRef = typeRef;
        const Type& type = typeRef.assertResolve(typeSystem);
        m_value = type.createValue(typeSystem);
    }
}

void babelwires::GenericValue::setValue(const TypeSystem& typeSystem, const ValueHolder& value) {
    assert(m_typeRef.assertResolve(typeSystem).isValidValue(typeSystem, *value));
    m_value = value;
}

std::size_t babelwires::GenericValue::getHash() const {
    // Arbitrary discriminator.
    std::size_t hash = 0x07070808;
    if (m_typeRef) {
        hash::mixInto(hash, m_typeRef, m_value);
    }
    return hash;
}

bool babelwires::GenericValue::operator==(const Value& other) const {
    if (const GenericValue* otherValue = other.as<GenericValue>()) {
        return (m_typeRef == otherValue->m_typeRef) && (m_value == otherValue->m_value);
    } else {
        return false;
    }
}