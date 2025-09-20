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

babelwires::TypeConstructor::TypeConstructorResult
babelwires::SumTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const TypeConstructorArguments& arguments,
                                              const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_typeArguments.size() < 2) {
        throw TypeSystemException() << "SumTypeConstructor expects at least 2 type arguments but got "
                                    << arguments.m_typeArguments.size();
    }
    unsigned int defaultIndex = 0;
    if (arguments.m_valueArguments.size() > 1) {
        throw TypeSystemException() << "SumTypeConstructor expects at most 1 value argument but got "
                                    << arguments.m_valueArguments.size();
    } else if (arguments.m_valueArguments.size() == 1) {
        if (const auto* intValue = arguments.m_valueArguments[0]->as<babelwires::IntValue>()) {
            defaultIndex = intValue->get();
            if (defaultIndex >= arguments.m_typeArguments.size()) {
                throw TypeSystemException() << "The default index provided to the SumTypeConstructor was out of range";
            }
        } else {
            throw TypeSystemException() << "The value argument provided to SumTypeConstructor was not an IntValue";
        }
    }

    return std::make_unique<ConstructedType<SumType>>(std::move(newTypeRef), arguments.m_typeArguments, defaultIndex);
}

babelwires::TypeRef babelwires::SumTypeConstructor::makeTypeRef(std::vector<TypeRef> types) {
    return TypeRef(getThisIdentifier(), {std::move(types), {}});
}
