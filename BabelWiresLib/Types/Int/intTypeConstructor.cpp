/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

std::tuple<babelwires::Range<babelwires::IntValue::NativeType>, babelwires::IntValue::NativeType>
babelwires::IntTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() != 3) {
        throw TypeSystemException() << "IntTypeConstructor expects 3 value arguments but got " << valueArguments.size();
    }

    IntValue::NativeType args[3];
    for (int i = 0; i < 3; ++i) {
        if (const IntValue* intValue = valueArguments[i]->as<IntValue>()) {
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Argument " << i << " given to IntTypeConstructor was not an IntValue";
        }
    }

    return {{args[0], args[1]}, args[2]};
}

std::unique_ptr<babelwires::Type>
babelwires::IntTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 0) {
        throw TypeSystemException() << "IntTypeConstructor does not expect type arguments but got "
                                    << typeArguments.size();
    }
    auto [range, defaultValue] = extractValueArguments(valueArguments);
    return std::make_unique<ConstructedType<IntType>>(std::move(newTypeRef), range, defaultValue);
}

babelwires::TypeRef babelwires::IntTypeConstructor::makeTypeRef(IntValue::NativeType min, IntValue::NativeType max, IntValue::NativeType defaultValue) {
    return TypeRef(getThisIdentifier(), IntValue(min), IntValue(max), IntValue(defaultValue));
}
