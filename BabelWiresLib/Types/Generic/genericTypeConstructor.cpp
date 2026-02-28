/**
 * A Type constructor which constructs a GenericType from a TypeExp and a number of type variables.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

babelwires::TypeExp babelwires::GenericTypeConstructor::makeTypeExp(TypeExp wrappedType, unsigned int numVariables) {
    return TypeExp(GenericTypeConstructor::getThisIdentifier(), {{wrappedType}, {IntValue(numVariables)}});
}

babelwires::ResultT<unsigned int> babelwires::GenericTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
    if (valueArguments.size() > 1) {
        return Error() << "GenericTypeConstructor expects 0 or 1 value arguments but got "
                       << valueArguments.size();
    } else if (valueArguments.empty()) {
        return 1; // Default to 1 variable if no argument is given.
    } else if (const IntValue* intValue = valueArguments[0]->tryAs<IntValue>()) {
        const IntValue::NativeType nativeValue = intValue->get();
        if (nativeValue < 0) {
            return Error() << "Value argument given to GenericTypeConstructor was negative";
        }
        if (nativeValue > std::numeric_limits<unsigned int>::max()) {
            return Error() << "Value argument given to GenericTypeConstructor was too large";
        }
        return static_cast<unsigned int>(nativeValue);
    } else {
        return Error() << "Value argument given to GenericTypeConstructor was not an IntValue";
    }
}

babelwires::ResultT<babelwires::TypePtr>
babelwires::GenericTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                                  const TypeConstructorArguments& arguments,
                                                  const std::vector<TypePtr>& resolvedTypeArguments) const {
    ASSIGN_OR_ERROR(auto numVariables, extractValueArguments(arguments.getValueArguments()));

    if (arguments.getTypeArguments().size() != 1) {
        return Error() << "GenericTypeConstructor expects 1 type argument but got "
                       << arguments.getTypeArguments().size();
    }
    return makeType<GenericType>(std::move(newTypeExp), resolvedTypeArguments[0], numVariables);
}
