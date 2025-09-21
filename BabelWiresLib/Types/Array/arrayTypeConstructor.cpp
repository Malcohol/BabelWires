/**
 * A TypeConstructor which constructs an array type with a particular entry type, size range and default size.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

std::tuple<unsigned int, unsigned int, unsigned int>
babelwires::ArrayTypeConstructor::extractValueArguments(const std::vector<EditableValueHolder>& valueArguments) {
    // TODO default size should be optional.
    if (valueArguments.size() != 3) {
        throw TypeSystemException() << "ArrayTypeConstructor expects 3 value arguments but got " << valueArguments.size();
    }

    IntValue::NativeType args[3];
    for (int i = 0; i < 3; ++i) {
        if (const IntValue* intValue = valueArguments[i]->as<IntValue>()) {
            const IntValue::NativeType nativeValue = intValue->get();
            if (nativeValue < 0) {
                throw TypeSystemException() << "Value argument " << i << " given to ArrayTypeConstructor was negative";
            }
            if (nativeValue > std::numeric_limits<unsigned int>::max()) {
                throw TypeSystemException() << "Value argument " << i << "given to ArrayTypeConstructor was too large";
            }
            args[i] = intValue->get();
        } else {
            throw TypeSystemException() << "Value argument " << i
                                        << " given to ArrayTypeConstructor was not an IntValue";
        }
    }

    if (args[1] < args[0]) {
        throw TypeSystemException() << "Trying to construct an array with a maximum size smaller than its minimum size";
    }

    if ((args[2] < args[0]) || (args[2] > args[1]))
    {
        throw TypeSystemException()
            << "Trying to construct an array with a default size outside its allowed size range";
    }

    return {static_cast<unsigned int>(args[0]), static_cast<unsigned int>(args[1]), static_cast<unsigned int>(args[2])};
}

babelwires::TypeConstructor::TypeConstructorResult
babelwires::ArrayTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const TypeConstructorArguments& arguments,
                                            const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_typeArguments.size() != 1) {
        throw TypeSystemException() << "ArrayTypeConstructor expects a single type arguments but got "
                                    << arguments.m_typeArguments.size();
    }
    auto [minimumSize, maximumSize, defaultSize] = extractValueArguments(arguments.m_valueArguments);

    return std::make_unique<ConstructedType<ArrayType>>(std::move(newTypeRef), arguments.m_typeArguments[0], minimumSize, maximumSize, defaultSize);
}

babelwires::TypeRef babelwires::ArrayTypeConstructor::makeTypeRef(TypeRef entryType, unsigned int minSize, unsigned int maxSize, unsigned int defaultSize) {
    assert(minSize <= maxSize);
    assert(defaultSize <= maxSize);
    if (defaultSize < minSize) {
        defaultSize = minSize;
    }
    return babelwires::TypeRef{getThisIdentifier(),
                                {{std::move(entryType)},
                                 {babelwires::IntValue(minSize), babelwires::IntValue(maxSize), babelwires::IntValue(defaultSize)}}};
}
