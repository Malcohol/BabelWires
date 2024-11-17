/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

std::tuple<babelwires::Range<babelwires::Rational>, babelwires::Rational>
babelwires::RationalTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() != 3) {
        throw TypeSystemException() << "RationalTypeConstructor expects 3 value arguments but got " << valueArguments.size();
    }

    Rational args[3];
    for (int i = 0; i < 3; ++i) {
        if (const RationalValue* intValue = valueArguments[i]->as<RationalValue>()) {
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Argument " << i << " given to RationalTypeConstructor was not an RationalValue";
        }
    }

    return {{args[0], args[1]}, args[2]};
}

std::unique_ptr<babelwires::Type>
babelwires::RationalTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 0) {
        throw TypeSystemException() << "RationalTypeConstructor does not expect type arguments but got "
                                    << typeArguments.size();
    }
    auto [range, defaultValue] = extractValueArguments(valueArguments);
    return std::make_unique<ConstructedType<RationalType>>(std::move(newTypeRef), range, defaultValue);
}

babelwires::TypeRef babelwires::RationalTypeConstructor::makeTypeRef(Rational min, Rational max, Rational defaultValue) {
    assert(min <= defaultValue);
    assert(defaultValue <= max);
    return TypeRef(getThisIdentifier(), RationalValue(min), RationalValue(max), RationalValue(defaultValue));
}
