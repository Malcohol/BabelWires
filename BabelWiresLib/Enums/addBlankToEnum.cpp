/**
 * A type constructor which adds a blank value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Enums/addBlankToEnum.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::ShortId babelwires::AddBlankToEnum::getBlankValue() {
    return BW_SHORT_ID("____", "____", "9e715cdc-399a-48b7-bc04-ad98e9e900d9");
}

std::unique_ptr<babelwires::Type> babelwires::AddBlankToEnum::constructType(TypeRef newTypeRef,
                                                                      const std::array<const Type*, 1>& arguments) const {
    const Enum* const srcEnum = arguments[0]->as<Enum>();
    if (!srcEnum) {
        throw TypeSystemException() << "Non-enum argument << " << arguments[0]->getName() << " provided to AddBlankToEnum";
    }
    return std::make_unique<ConstructedType<Enum>>(std::move(newTypeRef), ensureBlankValue(srcEnum->getEnumValues()), srcEnum->getIndexOfDefaultValue());
}

babelwires::Enum::EnumValues babelwires::AddBlankToEnum::ensureBlankValue(const Enum::EnumValues& srcValues) {
    if (srcValues.size() > 0) {
        if (srcValues[srcValues.size() - 1] == AddBlankToEnum::getBlankValue()) {
            return srcValues;
        }
        assert((std::find(srcValues.begin(), srcValues.end(), AddBlankToEnum::getBlankValue()) == srcValues.end()) &&
               "Found dummy value not at end of EnumValue vector.");
    }
    babelwires::Enum::EnumValues newValues = srcValues;
    newValues.emplace_back(AddBlankToEnum::getBlankValue());
    return newValues;
}


babelwires::SubtypeOrder babelwires::AddBlankToEnum::compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments<1>& arguments, const TypeRef& other) const {
    const SubtypeOrder argOrder = typeSystem.compareSubtype(arguments.m_typeArguments[0], other);
    if ((argOrder == SubtypeOrder::IsEquivalent) || (argOrder == SubtypeOrder::IsSupertype)) {
        return SubtypeOrder::IsSupertype;
    }
    return SubtypeOrder::IsUnrelated;
}

babelwires::SubtypeOrder babelwires::AddBlankToEnum::compareSubtypeHelper(const TypeSystem& typeSystem, const TypeConstructorArguments<1>& argumentsA, const TypeConstructorArguments<1>& argumentsB) const {
    return typeSystem.compareSubtype(argumentsA.m_typeArguments[0], argumentsB.m_typeArguments[0]);
}
