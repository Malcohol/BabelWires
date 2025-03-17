/**
 * TupleType
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

babelwires::TupleType::TupleType(ComponentTypes components)
    : m_componentTypes(std::move(components)) {
}

babelwires::NewValueHolder babelwires::TupleType::createValue(const TypeSystem& typeSystem) const {
    TupleValue::Tuple newTuple;
    newTuple.reserve(m_componentTypes.size());
    for (const auto& t : m_componentTypes) {
        const Type& type = t.resolve(typeSystem);
        newTuple.emplace_back(type.createValue(typeSystem).is<EditableValue>());
    }
    return babelwires::ValueHolder::makeValue<TupleValue>(std::move(newTuple));
}

bool babelwires::TupleType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const TupleValue* tuple = v.as<TupleValue>();
    if (!tuple) {
        return false;
    }
    if (tuple->getSize() != m_componentTypes.size()) {
        return false;
    }
    for (unsigned int i = 0; i < m_componentTypes.size(); ++i) {
        const Type& type = m_componentTypes[i].resolve(typeSystem);
        if (!type.isValidValue(typeSystem, *tuple->getValue(i))) {
            return false;
        }
    }
    return true;
};

std::string babelwires::TupleType::getKind() const {
    return "tuple";
}

const babelwires::TupleType::ComponentTypes& babelwires::TupleType::getComponentTypes() const {
    return m_componentTypes;
}

babelwires::SubtypeOrder babelwires::TupleType::compareSubtypeHelper(
    const TypeSystem& typeSystem, const Type& other) const {
    
    const TupleType* const otherTupleType = other.as<TupleType>();
    if (!otherTupleType) {
        return SubtypeOrder::IsUnrelated;
    }
    const std::vector<TypeRef>& parametersA = getComponentTypes();
    const std::vector<TypeRef>& parametersB = otherTupleType->getComponentTypes();
    
    if (parametersA.size() != parametersB.size()) {
        return SubtypeOrder::IsUnrelated;
    }
    SubtypeOrder order = typeSystem.compareSubtype(parametersA[0], parametersB[0]);
    if (order == SubtypeOrder::IsUnrelated) {
        return SubtypeOrder::IsUnrelated;
    }
    for (int i = 1; i < parametersA.size(); ++i) {
        const SubtypeOrder currentOrder = typeSystem.compareSubtype(parametersA[i], parametersB[i]);
        if ((currentOrder != SubtypeOrder::IsEquivalent) && (currentOrder != order)) {
            if (order == SubtypeOrder::IsEquivalent) {
                order = currentOrder;
            } else {
                return SubtypeOrder::IsUnrelated;
            }
        }
    }
    return order;
}

std::string babelwires::TupleType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    const TupleValue& tuple = v->is<TupleValue>();
    assert(isValidValue(typeSystem, *v));
    std::ostringstream os;
    os << "(";
    std::string sep = "";
    for (int i = 0; i < m_componentTypes.size(); ++i) {
        os << sep;
        sep = ", ";
        const Type& type = m_componentTypes[i].resolve(typeSystem);
        os << type.valueToString(typeSystem, tuple.getValue(i));
    }
    os << ")";
    return os.str();
}
