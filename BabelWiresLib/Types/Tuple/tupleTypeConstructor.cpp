/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

std::unique_ptr<babelwires::Type>
babelwires::TupleTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (valueArguments.size() > 0) {
        throw TypeSystemException() << "TupleTypeConstructor does not expect value arguments but got "
                                    << valueArguments.size();
    }

    std::vector<TypeRef> operands;
    operands.reserve(operands.size());
    std::for_each(typeArguments.begin(), typeArguments.end(),
                  [&operands](const Type* t) { operands.emplace_back(t->getTypeRef()); });
    return std::make_unique<ConstructedType<TupleType>>(std::move(newTypeRef), operands);
}

babelwires::TypeRef babelwires::TupleTypeConstructor::makeTypeRef(std::vector<TypeRef> types) {
    return TypeRef(getThisIdentifier(), {std::move(types), {}});
}
