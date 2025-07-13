/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/genericValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::GenericType::GenericType(TypeRef wrappedType, unsigned int numVariables)
    : m_wrappedType(std::move(wrappedType))
    , m_numVariables(numVariables) {}

std::string babelwires::GenericType::getFlavour() const {
    // Not connectable.
    return "";
}

babelwires::NewValueHolder babelwires::GenericType::createValue(const TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<GenericValue>(typeSystem, m_wrappedType, m_numVariables);
}

bool babelwires::GenericType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const GenericValue* genericValue = v.as<GenericValue>();
    if (!genericValue) {
        return false;
    }

    // TODO
    return true;
}

unsigned int babelwires::GenericType::getNumChildren(const ValueHolder& compoundValue) const {
    return 1;
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    const GenericValue& genericValue = compoundValue->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getWrappedType()};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    GenericValue& genericValue = compoundValue.copyContentsAndGetNonConst().is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getWrappedType()};
}

int babelwires::GenericType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    assert(step == getStepToValue() && "The step is not the expected step for GenericType");
    assert(compoundValue->is<GenericValue>().getValue() && "The value is not a GenericValue");
    return 0;
}

std::optional<babelwires::SubtypeOrder> babelwires::GenericType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                      const Type& other) const {
    // TODO
    return SubtypeOrder::IsDisjoint;
}

std::string babelwires::GenericType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const GenericValue& genericValue = v->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child value is empty");
    const auto& value = genericValue.getValue();
    const Type& baseType = genericValue.getWrappedType().assertResolve(typeSystem);
    return baseType.valueToString(typeSystem, value);
}

babelwires::ShortId babelwires::GenericType::getStepToValue() {
    return BW_SHORT_ID("wrappd", "value", "69d92618-a000-476e-afc1-9121e1bfac1e");
}

void babelwires::GenericType::instantiate(const TypeSystem& typeSystem, ValueHolder& genericValue, unsigned int variableIndex, const TypeRef& typeValue) const {
    GenericValue& value = genericValue.copyContentsAndGetNonConst().is<GenericValue>();
    value.assignTypeVariable(variableIndex, typeValue);
    value.instantiate(typeSystem, m_wrappedType);
}
