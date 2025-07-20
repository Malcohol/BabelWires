/**
 * A type constructor which constructs an EnumType from two other EnumTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumUnionTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <unordered_set>

babelwires::TypeConstructor::TypeConstructorResult
babelwires::EnumUnionTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                   const std::vector<const Type*>& typeArguments,
                                                   const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() < 2) {
        throw TypeSystemException() << "EnumUnionTypeConstructor expects two or more types as arguments, but got "
                                    << typeArguments.size();
    }
    if (valueArguments.size() != 0) {
        throw TypeSystemException() << "EnumUnionTypeConstructor does not expect value arguments, but got "
                                    << valueArguments.size();
    }
    // We build the unionOfValues in order, skipping duplicates when encountered.
    EnumType::ValueSet unionOfValues;
    std::unordered_set<ShortId> setOfValuesSeen;
    for (unsigned int i = 0; i < typeArguments.size(); ++i) {
        const EnumType* const enumType = typeArguments[i]->as<EnumType>();
        if (!enumType) {
            throw TypeSystemException() << "Argument number " << i << " passed to EnumTypeConstructor was not an enum type (" << typeArguments[i] << ")";
        }
        for (auto v : enumType->getValueSet()) {
            const auto [_, wasInserted] = setOfValuesSeen.insert(v);
            if (wasInserted) {
                unionOfValues.emplace_back(v);
            }
        }
    }
    return std::make_unique<ConstructedType<EnumType>>(std::move(newTypeRef), std::move(unionOfValues), 0);
}

babelwires::TypeRef babelwires::EnumUnionTypeConstructor::makeTypeRef(TypeRef enumA, TypeRef enumB) {
    return TypeRef{getThisIdentifier(), std::move(enumA), std::move(enumB)};
}
