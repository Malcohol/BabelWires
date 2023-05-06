/**
 * An EnumFeature exposes an enum in the project, and allows the user to make a choice from a set of named values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::EnumFeature::EnumFeature(TypeRef e)
    : SimpleValueFeature(std::move(e)) {}

const babelwires::Enum& babelwires::EnumFeature::getEnum() const {
    return getType().is<Enum>();
}

babelwires::ShortId babelwires::EnumFeature::get() const {
    return getValue().is<EnumValue>().get();
}

void babelwires::EnumFeature::set(ShortId id) {
    setValue(EnumValue(id));
}

int babelwires::EnumFeature::tryGetEnumIndex() const {
    return getType().is<Enum>().tryGetIndexFromIdentifier(get());
}

void babelwires::EnumFeature::setToEnumIndex(unsigned int index) {
    set(getType().is<Enum>().getIdentifierFromIndex(index));
}
