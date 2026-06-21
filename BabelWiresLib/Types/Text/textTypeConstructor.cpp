/**
 * A TypeConstructor which constructs a text type with a given maximum length.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Text/textTypeConstructor.hpp>

#include <BabelWiresLib/Types/Text/textType.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

babelwires::ResultT<babelwires::IntValue::NativeType>
babelwires::TextTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
    if (valueArguments.size() != 1) {
        return Error() << "TextTypeConstructor expects 1 value argument but got " << valueArguments.size();
    }

    if (const IntValue* const intValue = valueArguments[0]->tryAs<IntValue>()) {
        return intValue->get();
    } else {
        return Error() << "Argument 0 given to TextTypeConstructor was not an IntValue";
    }
}

babelwires::ResultT<babelwires::TypePtr>
babelwires::TextTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                               const TypeConstructorArguments& arguments,
                                               const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 0) {
        return Error() << "TextTypeConstructor does not expect type arguments but got "
                       << arguments.getTypeArguments().size();
    }
    ASSIGN_OR_ERROR(auto maxLength, extractValueArguments(arguments.getValueArguments()));
    return makeType<TextType>(std::move(newTypeExp), maxLength);
}

babelwires::TypeExp babelwires::TextTypeConstructor::makeTypeExp(IntValue::NativeType maxLength) {
    return TypeExp(getThisIdentifier(), IntValue(maxLength));
}
