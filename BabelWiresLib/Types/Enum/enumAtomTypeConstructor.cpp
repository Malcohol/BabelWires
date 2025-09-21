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

babelwires::TypeConstructor::TypeConstructorResult
babelwires::EnumAtomTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_typeArguments.size() != 0) {
        throw TypeSystemException() << "EnumAtomTypeConstructor does not expect any type arguments but got "
                                    << arguments.m_typeArguments.size();
    }
    if (arguments.m_valueArguments.size() != 1) {
        throw TypeSystemException() << "EnumAtomTypeConstructor expects a single value argument but got "
                                    << arguments.m_valueArguments.size();
    }
    const EnumValue* const enumValue = arguments.m_valueArguments[0]->as<EnumValue>();
    if (!enumValue) {
        throw TypeSystemException() << "Non-EnumValue argument << " << arguments.m_valueArguments[0] << " provided to EnumAtomTypeConstructor";
    }
    return std::make_unique<ConstructedType<EnumType>>(std::move(newTypeRef), EnumType::ValueSet{enumValue->get()}, 0);
}

babelwires::TypeRef babelwires::EnumAtomTypeConstructor::makeTypeRef(EnumValue enumValue) {
    return TypeRef{getThisIdentifier(), std::move(enumValue)};
}
