/**
 * Construct a FailureType with a context-specific error message.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Failure/failureTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

babelwires::TypeConstructor::TypeConstructorResult
babelwires::FailureTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                  const TypeConstructorArguments& arguments,
                                                  const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 0) {
        throw TypeSystemException() << "FailureTypeConstructor does not expect type arguments but got "
                                    << arguments.getTypeArguments().size();
    }

    if (arguments.getValueArguments().size() != 1) {
        throw TypeSystemException() << "IntTypeConstructor expects a single value argument but got "
                                    << arguments.getValueArguments().size();
    }

    if (const StringValue* message = arguments.getValueArguments()[0]->as<StringValue>()) {
        return std::make_unique<ConstructedType<FailureType>>(std::move(newTypeRef), message->get());
    } else {
        throw TypeSystemException() << "The argument given to FailureTypeConstructor was not an StringValue";
    }
}

babelwires::TypeRef babelwires::FailureTypeConstructor::makeTypeRef(std::string message) {
    return TypeRef(getThisIdentifier(), {{StringValue(std::move(message))}});
}
