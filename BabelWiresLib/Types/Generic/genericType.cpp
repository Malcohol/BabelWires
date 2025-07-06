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

babelwires::GenericType::GenericType(TypeRef baseType)
    : m_baseType(std::move(baseType)) {}

std::string babelwires::GenericType::getFlavour() const {
    return "generic";
}

babelwires::NewValueHolder babelwires::GenericType::createValue(const TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<GenericValue>();
}

bool babelwires::GenericType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    const GenericValue* genericValue = v.as<GenericValue>();
    if (!genericValue) {
        return false;
    }
    if (const auto& value = genericValue->getValue()) {
        if (m_baseType) {
            if (typeSystem.isSubType(genericValue->getTypeRef(), m_baseType)) {
                assert(genericValue->getTypeRef().assertResolve(typeSystem).isValidValue(typeSystem, *value) &&
                       "Encountered a generic value whose value didn't match its type.");
                return true; // The value is of a type that is a subtype of the base type.
            } else {
                return false;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}

unsigned int babelwires::GenericType::getNumChildren(const ValueHolder& compoundValue) const {
    const GenericValue& genericValue = compoundValue->is<GenericValue>();
    return genericValue.getValue() ? 1 : 0;
}

std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChild(const ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    const GenericValue& genericValue = compoundValue->is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child requested but value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getTypeRef()};
}

std::tuple<babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>
babelwires::GenericType::getChildNonConst(ValueHolder& compoundValue, unsigned int i) const {
    assert(i == 0 && "GenericType only has one child");
    GenericValue& genericValue = compoundValue.copyContentsAndGetNonConst().is<GenericValue>();
    assert(genericValue.getValue() && "GenericType child requested but value is empty");
    return {&genericValue.getValue(), getStepToValue(), genericValue.getTypeRef()};
}

int babelwires::GenericType::getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const {
    assert(step == getStepToValue() && "The step is not the expected step for GenericType");
    assert(compoundValue->is<GenericValue>().getValue() && "The value is not a GenericValue");
    return 0;
}

std::optional<babelwires::SubtypeOrder> babelwires::GenericType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                                      const Type& other) const {
    // TODO
    return SubtypeOrder::IsIntersecting;
}

std::string babelwires::GenericType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    const GenericValue& genericValue = v->is<GenericValue>();
    const auto& value = genericValue.getValue();
    if (!value) {
        return "empty";
    }
    const Type& baseType = genericValue.getTypeRef().assertResolve(typeSystem);
    return baseType.valueToString(typeSystem, value);
}

babelwires::ShortId babelwires::GenericType::getStepToValue() {
    return BW_SHORT_ID("value", "value", "69d92618-a000-476e-afc1-9121e1bfac1e");
}
