/**
 * A type constructor which creates an enum with a single value.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>

babelwires::TypePtr
babelwires::EnumAtomTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeRef,
                                                   const TypeConstructorArguments& arguments,
                                                   const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 0) {
        throw TypeSystemException() << "EnumAtomTypeConstructor does not expect any type arguments but got "
                                    << arguments.getTypeArguments().size();
    }
    if (arguments.getValueArguments().size() != 1) {
        throw TypeSystemException() << "EnumAtomTypeConstructor expects a single value argument but got "
                                    << arguments.getValueArguments().size();
    }
    const EnumValue* const enumValue = arguments.getValueArguments()[0]->as<EnumValue>();
    if (!enumValue) {
        throw TypeSystemException() << "Non-EnumValue argument << " << arguments.getValueArguments()[0]
                                    << " provided to EnumAtomTypeConstructor";
    }
    return makeType<ConstructedType<EnumType>>(std::move(newTypeRef), EnumType::ValueSet{enumValue->get()}, 0);
}

babelwires::TypeExp babelwires::EnumAtomTypeConstructor::makeTypeExp(EnumValue enumValue) {
    return TypeExp{getThisIdentifier(), std::move(enumValue)};
}
