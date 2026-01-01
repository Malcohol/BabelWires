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

babelwires::TypePtr
babelwires::TupleTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeRef,
                                                const TypeConstructorArguments& arguments,
                                                const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getValueArguments().size() > 0) {
        throw TypeSystemException() << "TupleTypeConstructor does not expect value arguments but got "
                                    << arguments.getValueArguments().size();
    }

    return makeType<ConstructedType<TupleType>>(std::move(newTypeRef), arguments.getTypeArguments());
}

babelwires::TypeExp babelwires::TupleTypeConstructor::makeTypeExp(std::vector<TypeExp> types) {
    return TypeExp(getThisIdentifier(), {std::move(types), {}});
}
