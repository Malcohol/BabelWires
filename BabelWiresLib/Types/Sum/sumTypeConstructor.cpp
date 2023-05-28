/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>

std::unique_ptr<babelwires::Type>
babelwires::SumTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() < 2) {
        throw TypeSystemException() << "SumTypeConstructor expects at least 2 type arguments but got " << typeArguments.size();
    }
    if (valueArguments.size() != 0) {
        throw TypeSystemException() << "SumTypeConstructor does not expect any value arguments but got " << valueArguments.size();
    }

    std::vector<TypeRef> summands;
    summands.reserve(typeArguments.size());
    std::for_each(typeArguments.begin(), typeArguments.end(), [&summands](const Type* t){ summands.emplace_back(t->getTypeRef());});
    return std::make_unique<ConstructedType<SumType>>(std::move(newTypeRef), summands);
}

babelwires::SubtypeOrder
babelwires::SumTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                     const TypeConstructorArguments& argumentsA,
                                                     const TypeConstructorArguments& argumentsB) const {
    if ((argumentsA.m_valueArguments.size() != 0) || (argumentsB.m_valueArguments.size() != 0)) {
        return SubtypeOrder::IsUnrelated;
    }
    return compareSubtypeHelperInternal(typeSystem, argumentsA.m_typeArguments, argumentsB.m_typeArguments);
}

babelwires::SubtypeOrder babelwires::SumTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const TypeConstructorArguments& arguments,
                                                                              const TypeRef& other) const {
    const SumType* const otherSumType = other.tryResolve(typeSystem)->as<SumType>();
    if (!otherSumType) {
        return SubtypeOrder::IsUnrelated;
    } else {
        return compareSubtypeHelperInternal(typeSystem, arguments.m_typeArguments, otherSumType->getSummands());
    }
}

babelwires::SubtypeOrder babelwires::SumTypeConstructor::compareSubtypeHelperInternal(
    const TypeSystem& typeSystem, const std::vector<TypeRef>& summandsA, const std::vector<TypeRef>& summandsB) const {
    if ((summandsA.size() <= 2) || (summandsB.size() != summandsA.size())) {
        return SubtypeOrder::IsUnrelated;
    }
    SubtypeOrder order = typeSystem.compareSubtype(summandsA[0], summandsB[0]);
    if (order == SubtypeOrder::IsUnrelated) {
        return SubtypeOrder::IsUnrelated;
    }
    for (int i = 1; i < summandsA.size(); ++i) {
        const SubtypeOrder currentOrder = typeSystem.compareSubtype(summandsA[i], summandsB[i]);
        if ((currentOrder != SubtypeOrder::IsEquivalent) && (currentOrder != order)) {
            if (order == SubtypeOrder::IsEquivalent) {
                order = currentOrder;
            } else {
                return SubtypeOrder::IsUnrelated;
            }
        }
    }
    return order;
}
