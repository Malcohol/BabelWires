/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

std::tuple<babelwires::Range<babelwires::IntValue::NativeType>, babelwires::IntValue::NativeType>
babelwires::IntTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() != 3) {
        throw TypeSystemException() << "IntTypeConstructor expects 3 value arguments but got " << valueArguments.size();
    }

    IntValue::NativeType args[3];
    for (int i = 0; i < 3; ++i) {
        if (const IntValue* intValue = valueArguments[i]->as<IntValue>()) {
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Argument " << i << " given to IntTypeConstructor was not an IntValue";
        }
    }

    return {{args[0], args[1]}, args[2]};
}

std::unique_ptr<babelwires::Type>
babelwires::IntTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 0) {
        throw TypeSystemException() << "IntTypeConstructor does not expect type arguments but got "
                                    << typeArguments.size();
    }
    auto [range, defaultValue] = extractValueArguments(valueArguments);
    return std::make_unique<ConstructedType<IntType>>(std::move(newTypeRef), range, defaultValue);
}

babelwires::SubtypeOrder
babelwires::IntTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                     const TypeConstructorArguments& argumentsA,
                                                     const TypeConstructorArguments& argumentsB) const {
    // TODO handle malformed arguments.
    auto [rangeA, _] = extractValueArguments(argumentsA.m_valueArguments);
    auto [rangeB, __] = extractValueArguments(argumentsB.m_valueArguments);

    const bool aSubB = rangeB.contains(rangeA);
    const bool bSubA = rangeA.contains(rangeB);

    if (aSubB && bSubA) {
        return SubtypeOrder::IsEquivalent;
    } else if (aSubB) {
        return SubtypeOrder::IsSubtype;
    } else if (bSubA) {
        return SubtypeOrder::IsSupertype;
    } else {
        return SubtypeOrder::IsUnrelated;
    }
}

babelwires::SubtypeOrder babelwires::IntTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const TypeConstructorArguments& arguments,
                                                                              const TypeRef& other) const {
    const Type *const type = other.tryResolve(typeSystem);
    if (!type) {
        return SubtypeOrder::IsUnrelated;
    }
    const IntType *const intType = type->as<IntType>();
    if (!intType) {
        return SubtypeOrder::IsUnrelated;
    }

    auto [rangeThis, _] = extractValueArguments(arguments.m_valueArguments);
    const auto& rangeOther = intType->getRange();

    const bool thisSubOther = rangeOther.contains(rangeThis);
    const bool otherSubThis = rangeThis.contains(rangeOther);

    if (thisSubOther && otherSubThis) {
        return SubtypeOrder::IsEquivalent;
    } else if (thisSubOther) {
        return SubtypeOrder::IsSubtype;
    } else if (otherSubThis) {
        return SubtypeOrder::IsSupertype;
    } else {
        return SubtypeOrder::IsUnrelated;
    }

    return SubtypeOrder::IsEquivalent;
}
