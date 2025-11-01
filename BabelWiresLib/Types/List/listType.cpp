/**
 * ListType is the type for ListValue.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/List/listType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/List/listValue.hpp>

babelwires::ListType::ListType(TypeRef elementTypeRef)
    : m_elementTypeRef(std::move(elementTypeRef)) {}

babelwires::NewValueHolder babelwires::ListType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<ListValue>(typeSystem, m_elementTypeRef);
}

bool babelwires::ListType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const ListValue* const list = v.as<ListValue>()) {
        return typeSystem.isSubType(list->getElementTypeRef(), m_elementTypeRef);
    }
    return false;
}

std::string babelwires::ListType::getFlavour() const {
    return ListValue::serializationType;
}

const babelwires::TypeRef& babelwires::ListType::getElementTypeRef() const {
    return m_elementTypeRef;
}

std::optional<babelwires::SubtypeOrder> babelwires::ListType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                   const Type& other) const {
    const ListType* const otherListType = other.as<ListType>();
    if (!otherListType) {
        return {};
    }
    return typeSystem.compareSubtype(m_elementTypeRef, otherListType->m_elementTypeRef);
}

std::string babelwires::ListType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    return v->is<ListValue>().toString();
}