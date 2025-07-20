/**
 * A Type constructor which constructs a GenericType from a TypeRef and a number of type variables.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

babelwires::TypeRef babelwires::GenericTypeConstructor::makeTypeRef(unsigned int numVariables) {
    return TypeRef(GenericTypeConstructor::getThisIdentifier(), IntValue(numVariables));
}

unsigned int
babelwires::GenericTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() > 1) {
        throw TypeSystemException() << "GenericTypeConstructor expects 0 or 1 value arguments but got "
                                    << valueArguments.size();
    } else if (valueArguments.empty()) {
        return 1; // Default to 1 variable if no argument is given.
    } else if (const IntValue* intValue = valueArguments[0]->as<IntValue>()) {
        const IntValue::NativeType nativeValue = intValue->get();
        if (nativeValue < 0) {
            throw TypeSystemException() << "Value argument given to GenericTypeConstructor was negative";
        }
        if (nativeValue > std::numeric_limits<unsigned int>::max()) {
            throw TypeSystemException() << "Value argument given to GenericTypeConstructor was too large";
        }
        return static_cast<unsigned int>(nativeValue);
    } else {
        throw TypeSystemException() << "Value argument given to GenericTypeConstructor was not an IntValue";
    }
}

babelwires::TypeConstructor::TypeConstructorResult
babelwires::GenericTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                  const TypeConstructorArguments& arguments,
                                                  const std::vector<const Type*>& resolvedTypeArguments) const {
    unsigned int numVariables = extractValueArguments(arguments.m_valueArguments);

    if (arguments.m_typeArguments.size() != 1) {
        throw TypeSystemException() << "GenericTypeConstructor expects 1 type argument but got "
                                    << arguments.m_typeArguments.size();
    }
    const TypeRef& wrappedType = arguments.m_typeArguments[0];

    return std::make_unique<ConstructedType<GenericType>>(std::move(newTypeRef), wrappedType, numVariables);
}
