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

babelwires::TypeRef babelwires::GenericTypeConstructor::makeTypeRef(TypeRef wrappedType, unsigned int numVariables) {
    return TypeRef(GenericTypeConstructor::getThisIdentifier(), {{wrappedType}, {IntValue(numVariables)}});
}

unsigned int babelwires::GenericTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
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
    unsigned int numVariables = extractValueArguments(arguments.getValueArguments());

    if (arguments.getTypeArguments().size() != 1) {
        throw TypeSystemException() << "GenericTypeConstructor expects 1 type argument but got "
                                    << arguments.getTypeArguments().size();
    }
    const TypeRef& wrappedType = arguments.getTypeArguments()[0];

    return makeType<ConstructedType<GenericType>>(std::move(newTypeRef), wrappedType, numVariables);
}
