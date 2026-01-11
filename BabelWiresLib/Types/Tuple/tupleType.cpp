/**
 * TupleType
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

babelwires::TupleType::TupleType(TypeExp&& typeExpOfThis, const TypeSystem& typeSystem, ComponentTypesExps components)
    : Type(std::move(typeExpOfThis)) {
    m_componentTypes.reserve(components.size());
    for (const auto& c : components) {
        m_componentTypes.emplace_back(c.resolve(typeSystem));
    }
}

babelwires::TupleType::TupleType(TypeExp&& typeExpOfThis, ComponentTypes components)
    : Type(std::move(typeExpOfThis))
    , m_componentTypes(std::move(components)) {
}

babelwires::NewValueHolder babelwires::TupleType::createValue(const TypeSystem& typeSystem) const {
    TupleValue::Tuple newTuple;
    newTuple.reserve(m_componentTypes.size());
    for (const auto& t : m_componentTypes) {
        newTuple.emplace_back(t->createValue(typeSystem));
    }
    return babelwires::ValueHolder::makeValue<TupleValue>(std::move(newTuple));
}

bool babelwires::TupleType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    const TupleValue* tuple = v.tryAs<TupleValue>();
    if (!tuple) {
        return false;
    }
    if (tuple->getSize() != m_componentTypes.size()) {
        return false;
    }
    for (unsigned int i = 0; i < m_componentTypes.size(); ++i) {
        if (!visitor(typeSystem, m_componentTypes[i], *tuple->getValue(i), i)) {
            return false;
        }
    }
    return true;
};

std::string babelwires::TupleType::getFlavour() const {
    return "tuple";
}

const babelwires::TupleType::ComponentTypes& babelwires::TupleType::getComponentTypes() const {
    return m_componentTypes;
}

std::optional<babelwires::SubtypeOrder> babelwires::TupleType::compareSubtypeHelper(
    const TypeSystem& typeSystem, const Type& other) const {
    
    const TupleType* const otherTupleType = other.tryAs<TupleType>();
    if (!otherTupleType) {
        return {};
    }
  
    if (m_componentTypes.size() != otherTupleType->m_componentTypes.size()) {
        return SubtypeOrder::IsDisjoint;
    }
    SubtypeOrder order = SubtypeOrder::IsEquivalent;
    for (int i = 0; i < m_componentTypes.size(); ++i) {
        const SubtypeOrder componentOrder = typeSystem.compareSubtype(*m_componentTypes[i], *otherTupleType->m_componentTypes[i]);
        order = subtypeProduct(order, componentOrder);
    }
    return order;
}

std::string babelwires::TupleType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    const TupleValue& tuple = v->as<TupleValue>();
    assert(isValidValue(typeSystem, *v));
    std::ostringstream os;
    os << "(";
    std::string sep = "";
    for (int i = 0; i < m_componentTypes.size(); ++i) {
        os << sep;
        sep = ", ";
        os << m_componentTypes[i]->valueToString(typeSystem, tuple.getValue(i));
    }
    os << ")";
    return os.str();
}
