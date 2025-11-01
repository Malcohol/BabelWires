/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/List/ListProject/listSerialization.hpp>

babelwires::ListBundle::ListBundle(std::filesystem::path pathToFile, MapValue&& map)
    : DataBundle(std::move(pathToFile), std::move(map)) {}

void babelwires::ListBundle::visitIdentifiers(IdentifierVisitor& visitor) {
    getData().visitIdentifiers(visitor);
}

void babelwires::ListBundle::visitFilePaths(FilePathVisitor& visitor) {
    getData().visitFilePaths(visitor);
}
