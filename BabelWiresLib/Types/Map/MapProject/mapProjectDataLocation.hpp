/**
 * A MapProjectDataLocation identifies some data within a MapProject.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

namespace babelwires {
    /// A MapProjectDataLocation identifies some data within a MapProject.
    class MapProjectDataLocation : public DataLocation {
      public:
        CLONEABLE(MapProjectDataLocation);
        SERIALIZABLE(MapProjectDataLocation, "mapProjectLocation", void, 1);

        enum class Side {
            source,
            target
        };

        MapProjectDataLocation(unsigned int entryIndex, Side side, Path pathToValue);
        MapProjectDataLocation(const MapProjectDataLocation& other) = default;

        unsigned int getEntryIndex() const;
        Side getSide() const;
        const Path& getPath() const;

      public:
        // Non-virtual methods which give identity to the data just in terms of elementId and pathToValue.
        std::size_t getHash() const override;

      protected:
        bool equals(const DataLocation& other) const override;

        void writeToStream(std::ostream& os) const override;

      public:
        // Serialization.
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      private:
        MapProjectDataLocation();

      private:
        unsigned int m_entryIndex;
        Side m_side;
        Path m_pathToValue;
    };
}
