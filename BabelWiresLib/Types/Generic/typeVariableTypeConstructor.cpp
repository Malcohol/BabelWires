/**
 * A TypeConstructor which constructs an array type with a particular entry type, size range and default size.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

babelwires::TypeVariableData
babelwires::TypeVariableTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
    // TODO make optional
    if (valueArguments.size() != 2) {
        throw TypeSystemException() << "TypeVariableTypeConstructor expects 2 value arguments but got "
                                    << valueArguments.size();
    }

    IntValue::NativeType args[2];
    for (int i = 0; i < 2; ++i) {
        if (const IntValue* intValue = valueArguments[i]->tryAs<IntValue>()) {
            const IntValue::NativeType nativeValue = intValue->get();
            if (nativeValue < 0) {
                throw TypeSystemException()
                    << "Value argument " << i << " given to TypeVariableTypeConstructor was negative";
            }
            if (nativeValue > std::numeric_limits<unsigned int>::max()) {
                throw TypeSystemException()
                    << "Value argument " << i << "given to TypeVariableTypeConstructor was too large";
            }
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Value argument " << i
                                        << " given to TypeVariableTypeConstructor was not an IntValue";
        }
    }
    return {static_cast<unsigned int>(args[0]), static_cast<unsigned int>(args[1])};
}

babelwires::TypePtr
babelwires::TypeVariableTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                                       const TypeConstructorArguments& arguments,
                                                       const std::vector<TypePtr>& resolvedTypeArguments) const {
    // Verify the arguments are valid, but actually they don't need to be passed to the resulting types.
    /*VariableData variableData =*/extractValueArguments(arguments.getValueArguments());

    assert(resolvedTypeArguments.size() == arguments.getTypeArguments().size());
    if (arguments.getTypeArguments().size() > 1) {
        throw TypeSystemException() << "TypeVariableTypeConstructor expects 0 or 1 type arguments but got "
                                    << arguments.getTypeArguments().size();
    }
    if (arguments.getTypeArguments().size() == 1) {
        return resolvedTypeArguments[0];
    } else {
        return makeType<TypeVariableType>(std::move(newTypeExp));
    }
}

babelwires::TypeExp babelwires::TypeVariableTypeConstructor::makeTypeExp(unsigned int typeVariableIndex,
                                                                         unsigned int numGenericTypeLevels) {
    return babelwires::TypeExp{getThisIdentifier(), babelwires::IntValue(typeVariableIndex),
                               babelwires::IntValue(numGenericTypeLevels)};
}
