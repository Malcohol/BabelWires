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
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>

std::unique_ptr<babelwires::Type>
babelwires::SumTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() < 2) {
        throw TypeSystemException() << "SumTypeConstructor expects at least 2 type arguments but got "
                                    << typeArguments.size();
    }
    unsigned int defaultIndex = 0;
    if (valueArguments.size() > 1) {
        throw TypeSystemException() << "SumTypeConstructor expects at most 1 value argument but got "
                                    << valueArguments.size();
    } else if (valueArguments.size() == 1) {
        if (const auto* intValue = valueArguments[0]->as<babelwires::IntValue>()) {
            defaultIndex = intValue->get();
            if (defaultIndex >= typeArguments.size()) {
                throw TypeSystemException() << "The default index provided to the SumTypeConstructor was out of range";
            }
        } else {
            throw TypeSystemException() << "The value argument provided to SumTypeConstructor was not an IntValue";
        }
    }

    std::vector<TypeRef> summands;
    summands.reserve(typeArguments.size());
    std::for_each(typeArguments.begin(), typeArguments.end(),
                  [&summands](const Type* t) { summands.emplace_back(t->getTypeRef()); });
    return std::make_unique<ConstructedType<SumType>>(std::move(newTypeRef), summands, defaultIndex);
}

babelwires::TypeRef babelwires::SumTypeConstructor::makeTypeRef(std::vector<TypeRef> types) {
    return TypeRef(getThisIdentifier(), {std::move(types), {}});
}
