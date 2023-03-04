/**
 * A type constructor which adds a dummy value to an existing enum
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Enums/addDummy.hpp>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>

babelwires::EnumWithDummy::EnumWithDummy(TypeRef typeRef, const Enum& srcEnum)
    : m_typeRef(std::move(typeRef))
    , Enum(ensureDummyValue(srcEnum.getEnumValues()), srcEnum.getIndexOfDefaultValue()) {}

babelwires::Enum::EnumValues babelwires::EnumWithDummy::ensureDummyValue(const EnumValues& srcValues) {
    if (srcValues.size() > 0) {
        if (srcValues[srcValues.size() - 1] == AddDummy::getDummyValue()) {
            return srcValues;
        }
        assert((std::find(srcValues.begin(), srcValues.end(), AddDummy::getDummyValue()) == srcValues.end()) && "Found dummy value not at end of EnumValue vector.");
    }
    babelwires::Enum::EnumValues newValues = srcValues;
    newValues.emplace_back(AddDummy::getDummyValue());
    return newValues;
}

babelwires::TypeRef babelwires::EnumWithDummy::getTypeRef() const {
    return m_typeRef;
}

unsigned int babelwires::AddDummy::getArity() const {
    return 1;
}

babelwires::Identifier babelwires::AddDummy::getDummyValue() {
    return REGISTERED_ID("____", "____", "9e715cdc-399a-48b7-bc04-ad98e9e900d9");
}

std::unique_ptr<babelwires::Type> babelwires::AddDummy::constructType(TypeRef newTypeRef, const std::vector<const Type*>& arguments) const {
    assert((arguments.size() == 1) && "AddDummy is unary.");
    const Enum* const srcEnum = arguments[0]->as<Enum>();
    if (!srcEnum) {
        throw TypeSystemException() << "Non-enum argument << " << arguments[0]->getName() << " provided to AddDummy";
    }
    return std::make_unique<EnumWithDummy>(std::move(newTypeRef), *srcEnum);
}
