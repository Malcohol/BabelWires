/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/type.hpp>

bool babelwires::Type::verifySupertype(const Type& supertype) const {
    assert(false);
    return false;
}

bool babelwires::Type::isAbstract() const {
    return false;
}

std::string babelwires::Type::getName() const {
    return getTypeRef().toString();
}
