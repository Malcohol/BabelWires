/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <BaseLib/DataContext/dataBundle.hpp>
#include <BaseLib/DataContext/dataSerialization.hpp>

#include <filesystem>

namespace babelwires {
    class MapBundle : public DataBundle<MapValue> {
      public:
        SERIALIZABLE(MapBundle, "map", void, 1);
        MapBundle() = default;
        MapBundle(MapBundle&&) = default;
        MapBundle& operator=(MapBundle&&) = default;

        /// Construct a bundle from mapValue.
        /// The map is modified to make the bundle independent of the current system.
        MapBundle(std::filesystem::path pathToFile, MapValue&& map);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    class MapSerialization : public DataSerialization<MapBundle> {};
} // namespace babelwires
