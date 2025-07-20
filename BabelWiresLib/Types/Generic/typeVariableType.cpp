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
#include <Common/Utilities/unicodeUtils.hpp>

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
    return toString(getVariableData());
}

babelwires::TypeVariableTypeConstructor::VariableData babelwires::TypeVariableType::getVariableData() const {
    auto optVariableData = TypeVariableTypeConstructor::isTypeVariable(getTypeRef());
    assert(optVariableData && "A typeVariable had a typeRef that wasn't using a typeVariableTypeConstructor");
    return *optVariableData;
}

std::string babelwires::TypeVariableType::toString(TypeVariableTypeConstructor::VariableData variableData) {
    std::ostringstream os;
    assert(variableData.m_numGenericTypeLevels < c_maxGenericTypeLevels);
    os.put('T' + variableData.m_numGenericTypeLevels);
    writeUnicodeSubscript(os, variableData.m_typeVariableIndex);
    return os.str();
}
