/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableValue.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::TypeVariableType::TypeVariableType() = default;

std::string babelwires::TypeVariableType::getFlavour() const {
    return "variable";
}

babelwires::NewValueHolder babelwires::TypeVariableType::createValue(const TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<TypeVariableValue>();
}

bool babelwires::TypeVariableType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    return v.as<TypeVariableValue>();
}

std::optional<babelwires::SubtypeOrder> babelwires::TypeVariableType::compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const {
    // TODO: Since the subtype order is responsible for connectivity, this may need to be more sophisticated.
    return other.as<TypeVariableType>() ? SubtypeOrder::IsEquivalent : SubtypeOrder::IsIntersecting;
}

std::string babelwires::TypeVariableType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const {
    return getVariableData().toString();
}

babelwires::TypeVariableData babelwires::TypeVariableType::getVariableData() const {
    auto optVariableData = TypeVariableData::isTypeVariable(getTypeRef());
    assert(optVariableData && "A typeVariable had a typeRef that wasn't using a typeVariableTypeConstructor");
    return *optVariableData;
}
