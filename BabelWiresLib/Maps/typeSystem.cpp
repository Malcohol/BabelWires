/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/typeSystem.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::LongIdentifier babelwires::getBuiltInTypeId(KnownType type) {
    switch(type) {
        case KnownType::Int:
            return REGISTERED_LONGID("int", "integer", "90ed4c0c-2fa1-4373-9b67-e711358af824");
        default:
            assert(false && "The type was not a built-in");
    }
}

babelwires::KnownType babelwires::getTypeFromIdentifier(LongIdentifier id) {
    // Should I obtain the full id?
    if (id == "int") {
        return KnownType::Int;
    }
    else {
        return KnownType::Enum;
    }
}
