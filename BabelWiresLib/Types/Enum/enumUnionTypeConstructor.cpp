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

babelwires::TypePtr
babelwires::EnumUnionTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                                    const TypeConstructorArguments& arguments,
                                                    const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() < 2) {
        throw TypeSystemException() << "EnumUnionTypeConstructor expects two or more types as arguments, but got "
                                    << arguments.getTypeArguments().size();
    }
    if (arguments.getValueArguments().size() != 0) {
        throw TypeSystemException() << "EnumUnionTypeConstructor does not expect value arguments, but got "
                                    << arguments.getTypeArguments().size();
    }
    // We build the unionOfValues in order, skipping duplicates when encountered.
    EnumType::ValueSet unionOfValues;
    std::unordered_set<ShortId> setOfValuesSeen;
    for (unsigned int i = 0; i < resolvedTypeArguments.size(); ++i) {
        const EnumType* const enumType = resolvedTypeArguments[i]->tryAs<EnumType>();
        if (!enumType) {
            throw TypeSystemException() << "Argument number " << i
                                        << " passed to EnumTypeConstructor was not an enum type ("
                                        << arguments.getTypeArguments()[i] << ")";
        }
        for (auto v : enumType->getValueSet()) {
            const auto [_, wasInserted] = setOfValuesSeen.insert(v);
            if (wasInserted) {
                unionOfValues.emplace_back(v);
            }
        }
    }
    return makeType<EnumType>(std::move(newTypeExp), std::move(unionOfValues), 0);
}

babelwires::TypeExp babelwires::EnumUnionTypeConstructor::makeTypeExp(TypeExp enumA, TypeExp enumB) {
    return TypeExp{getThisIdentifier(), std::move(enumA), std::move(enumB)};
}
