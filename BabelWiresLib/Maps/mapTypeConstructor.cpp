/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapTypeConstructor.hpp>

#include <BabelWiresLib/Maps/mapType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

babelwires::MapEntryData::Kind
babelwires::MapTypeConstructor::extractValueArguments(const TypeSystem& typeSystem,
                                                      const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() != 1) {
        throw TypeSystemException() << "MapTypeConstructor expects 1 value argument but got " << valueArguments.size();
    }

    if (const EnumValue* enumValue = valueArguments[0]->as<EnumValue>()) {
        const MapEntryFallbackKind& mapEntryFallbackKind = typeSystem.getEntryByType<MapEntryFallbackKind>();
        return mapEntryFallbackKind.getValueFromIdentifier(enumValue->get());
    } else {
        throw TypeSystemException() << "Value argument 0 given to MapTypeConstructor was not a MapEntryFallbackKind";
    }
}

std::unique_ptr<babelwires::Type>
babelwires::MapTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got " << typeArguments.size();
    }
    babelwires::MapEntryData::Kind kind = extractValueArguments(typeSystem, valueArguments);
    return std::make_unique<ConstructedType<MapType>>(std::move(newTypeRef), typeArguments[0]->getTypeRef(),
                                                      typeArguments[1]->getTypeRef(), kind);
}

babelwires::SubtypeOrder
babelwires::MapTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                     const TypeConstructorArguments& argumentsA,
                                                     const TypeConstructorArguments& argumentsB) const {
    if (argumentsA.m_typeArguments.size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got "
                                    << argumentsA.m_typeArguments.size();
    }
    if (argumentsB.m_typeArguments.size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got "
                                    << argumentsB.m_typeArguments.size();
    }
    const SubtypeOrder orderSource =
        typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
    if (orderSource == SubtypeOrder::IsUnrelated) {
        return SubtypeOrder::IsUnrelated;
    }
    const SubtypeOrder orderTarget =
        typeSystem.compareSubtype(argumentsA.m_typeArguments[1], argumentsB.m_typeArguments[1]);
    return orderTarget;
}

babelwires::SubtypeOrder babelwires::MapTypeConstructor::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                              const TypeConstructorArguments& arguments,
                                                                              const TypeRef& other) const {
    const MapType* const otherMapType = other.tryResolve(typeSystem)->as<MapType>();
    if (!otherMapType) {
        return SubtypeOrder::IsUnrelated;
    } else {
        const SubtypeOrder orderSource =
            typeSystem.compareSubtype(arguments.m_typeArguments[0], otherMapType->getSourceTypeRef());
        if (orderSource == SubtypeOrder::IsUnrelated) {
            return SubtypeOrder::IsUnrelated;
        }
        const SubtypeOrder orderTarget =
            typeSystem.compareSubtype(arguments.m_typeArguments[1], otherMapType->getTargetTypeRef());
        return orderTarget;
    }
}
