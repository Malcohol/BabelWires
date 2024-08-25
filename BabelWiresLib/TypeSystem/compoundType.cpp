/**
 * CompoundTypes have values which have a structure made of child values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/compoundType.hpp>

bool babelwires::CompoundType::areDifferentNonRecursively(const ValueHolder& compoundValueA, const ValueHolder& compoundValueB) const {
    // Most compound types have no content separate from their children.
    return false;
}
