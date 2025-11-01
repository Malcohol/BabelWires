/**
 * A ListProjectDataLocation identifies some data within a ListProject.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

namespace babelwires {
    /// A ListProjectDataLocation identifies some data within a ListProject.
    class ListProjectDataLocation : public DataLocation {
      public:
        CLONEABLE(ListProjectDataLocation);
        SERIALIZABLE(ListProjectDataLocation, "listProjectLocation", void, 1);

        ListProjectDataLocation(unsigned int entryIndex, Path pathToValue);
        ListProjectDataLocation(const ListProjectDataLocation& other) = default;

        unsigned int getEntryIndex() const;

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

      private:
        ListProjectDataLocation() = default;

      private:
        unsigned int m_entryIndex;
    };
}
