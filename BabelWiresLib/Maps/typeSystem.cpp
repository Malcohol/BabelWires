/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/typeSystem.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::TypeSystem::TypeSystem(const EnumRegistry& enumRegistry)
    : m_enumRegistry(enumRegistry) {}

babelwires::LongIdentifier babelwires::TypeSystem::getBuiltInTypeId(Kind type) {
    switch (type) {
        case Kind::Int:
            return REGISTERED_LONGID("int", "integer", "90ed4c0c-2fa1-4373-9b67-e711358af824");
        default:
            assert(false && "The type was not a built-in");
    }
}

babelwires::TypeSystem::Kind babelwires::TypeSystem::getTypeFromIdentifier(LongIdentifier id) const {
    // Should I obtain the full id?
    if (id == "int") {
        return Kind::Int;
    } 

    if (m_enumRegistry.getEntryByIdentifier(id) != nullptr) {
        return Kind::Enum;
    }

    return Kind::NotAKind;
}

bool babelwires::TypeSystem::isAValue(Identifier id, LongIdentifier type) const {
    const Enum *const enumType = m_enumRegistry.getEntryByIdentifier(type);
    if (enumType == nullptr) {
        return false;
    }
    return enumType->isAValue(id);
}

bool babelwires::TypeSystem::isAValue(int value, LongIdentifier type) const {
    if (type == "int") {
        return true;
    }
    return false;
}
