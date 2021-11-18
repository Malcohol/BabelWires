/**
 * A Map defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/map.hpp>

bool babelwires::Map::operator==(const Map& other) const {
    return true;
}

bool babelwires::Map::operator!=(const Map& other) const {
    return true;
}

babelwires::LongIdentifier babelwires::Map::getSourceType() const {
    return "foo";
}

babelwires::LongIdentifier babelwires::Map::getTargetType() const {
    return "foo";
}

std::size_t babelwires::Map::getHash() const {
    return 4;
}
