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
#include <BabelWiresLib/Types/Int/intValue.hpp>

std::unique_ptr<babelwires::Type>
babelwires::IntTypeConstructor::constructType(TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                              const std::vector<ValueHolder>& valueArguments) const {
    if (valueArguments.size() != 3) {
        throw TypeSystemException() << "IntTypeConstructor expects 3 value arguments but got " << typeArguments.size();
    }
    if (typeArguments.size() != 0) {
        throw TypeSystemException() << "IntTypeConstructor does not expect type arguments but got "
                                    << typeArguments.size();
    }
    IntValue::NativeType args[3];
    for (int i = 0; i < 3; ++i) {
        if (const IntValue* intValue = valueArguments[i]->as<IntValue>()) {
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Argument " << i << " given to IntTypeConstructor was not an IntValue";
        }
    }
    return std::make_unique<ConstructedType<IntType>>(std::move(newTypeRef),
                                                      Range<IntValue::NativeType>{args[0], args[1]}, args[2]);
}

babelwires::SubtypeOrder
babelwires::IntTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                     const TypeConstructorArguments& argumentsA,
                                                     const TypeConstructorArguments& argumentsB) const {
    // TODO
    return SubtypeOrder::IsEquivalent;
}

babelwires::SubtypeOrder babelwires::IntTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const TypeConstructorArguments& arguments,
                                                                              const TypeRef& other) const {
    // TODO
    return SubtypeOrder::IsEquivalent;
}
