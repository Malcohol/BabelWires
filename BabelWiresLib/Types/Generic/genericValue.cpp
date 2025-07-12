/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericValue.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::GenericValue::GenericValue(TypeRef wrappedType, unsigned int numVariables)
    : m_actualWrappedType(wrappedType)
    , m_typeVariableAssignments(numVariables)
{
}


const babelwires::TypeRef& babelwires::GenericValue::getWrappedType() const {
    return m_actualWrappedType;
}

const babelwires::ValueHolder& babelwires::GenericValue::getValue() const {
    return m_wrappedValue;
}

babelwires::ValueHolder& babelwires::GenericValue::getValue() {
    return m_wrappedValue;
}

void babelwires::GenericValue::setValue(const TypeSystem& typeSystem, const ValueHolder& value) {
    assert(m_actualWrappedType.assertResolve(typeSystem).isValidValue(typeSystem, *value));
    m_wrappedValue = value;
}

std::size_t babelwires::GenericValue::getHash() const {
    auto hash = hash::mixtureOf(std::string("Generic"), m_actualWrappedType, m_wrappedValue);
    for (const auto& t : m_typeVariableAssignments) {
        hash::mixInto(hash, t);
    }
    return hash;
}

bool babelwires::GenericValue::operator==(const Value& other) const {
    if (const GenericValue* otherValue = other.as<GenericValue>()) {
        return (m_actualWrappedType == otherValue->m_actualWrappedType) && (m_wrappedValue == otherValue->m_wrappedValue);
    } else {
        return false;
    }
}