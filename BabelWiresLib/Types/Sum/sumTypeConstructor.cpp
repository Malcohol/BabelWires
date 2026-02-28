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

#include <BaseLib/Result/error.hpp>

babelwires::ResultT<babelwires::TypePtr>
babelwires::SumTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                              const TypeConstructorArguments& arguments,
                                              const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() < 2) {
        return Error() << "SumTypeConstructor expects at least 2 type arguments but got "
                       << arguments.getTypeArguments().size();
    }
    unsigned int defaultIndex = 0;
    if (arguments.getValueArguments().size() > 1) {
        return Error() << "SumTypeConstructor expects at most 1 value argument but got "
                       << arguments.getValueArguments().size();
    } else if (arguments.getValueArguments().size() == 1) {
        if (const auto* intValue = arguments.getValueArguments()[0]->tryAs<babelwires::IntValue>()) {
            defaultIndex = intValue->get();
            if (defaultIndex >= arguments.getTypeArguments().size()) {
                return Error() << "The default index provided to the SumTypeConstructor was out of range";
            }
        } else {
            return Error() << "The value argument provided to SumTypeConstructor was not an IntValue";
        }
    }

    return makeType<SumType>(std::move(newTypeExp), resolvedTypeArguments, defaultIndex);
}

babelwires::TypeExp babelwires::SumTypeConstructor::makeTypeExp(std::vector<TypeExp> types) {
    return TypeExp(getThisIdentifier(), {std::move(types), {}});
}
