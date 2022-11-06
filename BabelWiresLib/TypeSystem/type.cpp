/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

babelwires::Type::Type(LongIdentifier identifier, VersionNumber version)
    : RegistryEntry(identifier, version)
{}

bool babelwires::Type::verifySupertype(const Type& supertype) const {
    assert(false);
    return false;
}
