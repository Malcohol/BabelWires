/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

babelwires::ResultT<std::tuple<babelwires::Range<babelwires::IntValue::NativeType>, babelwires::IntValue::NativeType>>
babelwires::IntTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
    if (valueArguments.size() != 3) {
        return Error() << "IntTypeConstructor expects 3 value arguments but got " << valueArguments.size();
    }

    IntValue::NativeType args[3];
    for (int i = 0; i < 3; ++i) {
        if (const IntValue* intValue = valueArguments[i]->tryAs<IntValue>()) {
            args[i] = intValue->get();
        } else {
            return Error() << "Argument " << i << " given to IntTypeConstructor was not an IntValue";
        }
    }

    return std::tuple{Range<IntValue::NativeType>{args[0], args[1]}, args[2]};
}

babelwires::ResultT<babelwires::TypePtr>
babelwires::IntTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                              const TypeConstructorArguments& arguments,
                                              const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 0) {
        return Error() << "IntTypeConstructor does not expect type arguments but got "
                       << arguments.getTypeArguments().size();
    }
    ASSIGN_OR_ERROR(auto [range, defaultValue], extractValueArguments(arguments.getValueArguments()));
    return makeType<IntType>(std::move(newTypeExp), range, defaultValue);
}

babelwires::TypeExp babelwires::IntTypeConstructor::makeTypeExp(IntValue::NativeType min, IntValue::NativeType max,
                                                                IntValue::NativeType defaultValue) {
    return TypeExp(getThisIdentifier(), IntValue(min), IntValue(max), IntValue(defaultValue));
}
