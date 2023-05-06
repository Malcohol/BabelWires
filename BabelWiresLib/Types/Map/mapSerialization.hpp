/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/mapData.hpp>

#include <Common/DataContext/dataBundle.hpp>
#include <Common/DataContext/dataSerialization.hpp>

#include <filesystem>

namespace babelwires {
    class MapBundle : public DataBundle<MapData> {
      public:
        SERIALIZABLE(MapBundle, "map", void, 1);
        MapBundle() = default;
        MapBundle(MapBundle&&) = default;
        MapBundle& operator=(MapBundle&&) = default;

        /// Construct a bundle from mapData.
        /// The map is modified to make the bundle independent of the current system.
        MapBundle(std::filesystem::path pathToFile, MapData&& map);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    class MapSerialization : public DataSerialization<MapBundle> {};
} // namespace babelwires
