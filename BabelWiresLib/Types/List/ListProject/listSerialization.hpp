/**
 * Functions for serializing and deserializing a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <Common/DataContext/dataBundle.hpp>
#include <Common/DataContext/dataSerialization.hpp>

#include <filesystem>

namespace babelwires {
    class ListBundle : public DataBundle<MapValue> {
      public:
        SERIALIZABLE(ListBundle, "list", void, 1);
        ListBundle() = default;
        ListBundle(ListBundle&&) = default;
        ListBundle& operator=(ListBundle&&) = default;

        /// Construct a bundle from mapValue.
        /// The map is modified to make the bundle independent of the current system.
        ListBundle(std::filesystem::path pathToFile, MapValue&& map);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    class ListSerialization : public DataSerialization<ListBundle> {};
} // namespace babelwires
