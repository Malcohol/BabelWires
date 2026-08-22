/**
 * A TypeConstructor which constructs a fixed-point type.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Fixed/fixedTypeConstructor.hpp>

#include <BabelWiresLib/Types/Fixed/fixedType.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

babelwires::ResultT<std::tuple<int, babelwires::Range<babelwires::Fixed::NativeType>, babelwires::Fixed::NativeType>>
babelwires::FixedTypeConstructor::extractValueArguments(const std::vector<ValueHolder>& valueArguments) {
    if (valueArguments.size() != 4) {
        return Error() << "FixedTypeConstructor expects 4 value arguments but got " << valueArguments.size();
    }

    if (const IntValue* precisionValue = valueArguments[0]->tryAs<IntValue>()) {
        const int precision = static_cast<int>(precisionValue->get());
        if (precision < 0) {
            return Error() << "FixedTypeConstructor requires non-negative precision.";
        }

        const FixedValue* minFixedValue = valueArguments[1]->tryAs<FixedValue>();
        const FixedValue* maxFixedValue = valueArguments[2]->tryAs<FixedValue>();
        const FixedValue* defaultFixedValue = valueArguments[3]->tryAs<FixedValue>();
        if (!minFixedValue) {
            return Error() << "Argument 1 given to FixedTypeConstructor was not a FixedValue";
        }
        if (!maxFixedValue) {
            return Error() << "Argument 2 given to FixedTypeConstructor was not a FixedValue";
        }
        if (!defaultFixedValue) {
            return Error() << "Argument 3 given to FixedTypeConstructor was not a FixedValue";
        }

        const Fixed& minValue = minFixedValue->get();
        const Fixed& maxValue = maxFixedValue->get();
        const Fixed& defaultValue = defaultFixedValue->get();
        if (minValue.getPrecision() != precision || maxValue.getPrecision() != precision ||
            defaultValue.getPrecision() != precision) {
            return Error() << "FixedTypeConstructor precision arguments must match the declared precision.";
        }

        return std::tuple{precision, Range<Fixed::NativeType>{minValue.getNumerator(), maxValue.getNumerator()},
                          defaultValue.getNumerator()};
    }
    return Error() << "Argument 0 given to FixedTypeConstructor was not an IntValue";
}

babelwires::ResultT<babelwires::TypePtr>
babelwires::FixedTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                               const TypeConstructorArguments& arguments,
                                               const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 0) {
        return Error() << "FixedTypeConstructor does not expect type arguments but got "
                       << arguments.getTypeArguments().size();
    }
    ASSIGN_OR_ERROR(auto [precision, range, defaultValue], extractValueArguments(arguments.getValueArguments()));
    return makeType<FixedType>(std::move(newTypeExp), precision, range, defaultValue);
}

babelwires::TypeExp babelwires::FixedTypeConstructor::makeTypeExp(int precision, Fixed min, Fixed max, Fixed defaultValue) {
    assert(precision >= 0);
    assert(min.getPrecision() == precision);
    assert(max.getPrecision() == precision);
    assert(defaultValue.getPrecision() == precision);
    assert(min.getNumerator() <= defaultValue.getNumerator());
    assert(defaultValue.getNumerator() <= max.getNumerator());
    return TypeExp(getThisIdentifier(), babelwires::TypeConstructorArguments{{}, {IntValue(precision), FixedValue(min),
                                                                                 FixedValue(max), FixedValue(defaultValue)}});
}
