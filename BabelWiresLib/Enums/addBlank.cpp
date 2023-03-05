/**
 * A type constructor which adds a blank value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Enums/addBlank.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>

unsigned int babelwires::AddBlank::getArity() const {
    return 1;
}

babelwires::Identifier babelwires::AddBlank::getBlankValue() {
    return REGISTERED_ID("____", "____", "9e715cdc-399a-48b7-bc04-ad98e9e900d9");
}

std::unique_ptr<babelwires::Type> babelwires::AddBlank::constructType(TypeRef newTypeRef,
                                                                      const std::vector<const Type*>& arguments) const {
    assert((arguments.size() == 1) && "AddBlank is unary.");
    const Enum* const srcEnum = arguments[0]->as<Enum>();
    if (!srcEnum) {
        throw TypeSystemException() << "Non-enum argument << " << arguments[0]->getName() << " provided to AddBlank";
    }
    return std::make_unique<ConstructedType<Enum>>(std::move(newTypeRef), ensureBlankValue(srcEnum->getEnumValues()), srcEnum->getIndexOfDefaultValue());
}

babelwires::Enum::EnumValues babelwires::AddBlank::ensureBlankValue(const Enum::EnumValues& srcValues) {
    if (srcValues.size() > 0) {
        if (srcValues[srcValues.size() - 1] == AddBlank::getBlankValue()) {
            return srcValues;
        }
        assert((std::find(srcValues.begin(), srcValues.end(), AddBlank::getBlankValue()) == srcValues.end()) &&
               "Found dummy value not at end of EnumValue vector.");
    }
    babelwires::Enum::EnumValues newValues = srcValues;
    newValues.emplace_back(AddBlank::getBlankValue());
    return newValues;
}
