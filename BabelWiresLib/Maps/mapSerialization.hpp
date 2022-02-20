/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/map.hpp>
#include <BabelWiresLib/Serialization/dataBundle.hpp>
#include <BabelWiresLib/Serialization/dataSerialization.hpp>

#include <filesystem>

namespace babelwires {
    class MapBundle : public DataBundle<Map> {
      public:
        SERIALIZABLE(MapBundle, "map", void, 1);
        MapBundle() = default;
        MapBundle(MapBundle&&) = default;
        MapBundle& operator=(MapBundle&&) = default;

        /// Construct a bundle from projectData.
        /// The map is modified to make the bundle independent of the current system.
        MapBundle(std::filesystem::path pathToProjectFile, Map&& map);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    class MapSerialization : public DataSerialization<MapBundle> {};
} // namespace babelwires
