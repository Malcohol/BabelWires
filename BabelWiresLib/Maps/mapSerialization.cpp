/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapSerialization.hpp>

babelwires::MapBundle::MapBundle(std::filesystem::path pathToProjectFile, Map&& map)
    : DataBundle(std::move(pathToProjectFile), std::move(map)) {}

void babelwires::MapBundle::visitIdentifiers(IdentifierVisitor& visitor) {
    getData().visitIdentifiers(visitor);
}

void babelwires::MapBundle::visitFilePaths(FilePathVisitor& visitor) {
    getData().visitFilePaths(visitor);
}
