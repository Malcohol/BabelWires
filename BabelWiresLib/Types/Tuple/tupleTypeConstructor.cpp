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

babelwires::TypeConstructor::TypeConstructorResult
babelwires::TupleTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                                const TypeConstructorArguments& arguments,
                                                const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_valueArguments.size() > 0) {
        throw TypeSystemException() << "TupleTypeConstructor does not expect value arguments but got "
                                    << arguments.m_valueArguments.size();
    }

    return std::make_unique<ConstructedType<TupleType>>(std::move(newTypeRef), arguments.m_typeArguments);
}

babelwires::TypeRef babelwires::TupleTypeConstructor::makeTypeRef(std::vector<TypeRef> types) {
    return TypeRef(getThisIdentifier(), {std::move(types), {}});
}
