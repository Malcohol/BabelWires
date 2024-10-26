/**
 * A DataLocation identifies some data within the system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Features/Path/path.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Serialization/serializable.hpp>

namespace babelwires {
    /// A DataLocation identifies some data within the system.
    /// Right now it is a concrete class and identifies a row within the project.
    /// In theory, it should be abstracted and be able to identify data in other contexts
    /// as well. An example would be the source or target values in the MapEditor.
    class DataLocation : public Serializable, ProjectVisitable {
      public:
        SERIALIZABLE(DataLocation, "location", void, 1);

        DataLocation() = default;
        DataLocation(ElementId elementId, Path pathToValue);
        DataLocation(const DataLocation& other) = default;

        ElementId getElementId() const;
        const Path& getPathToValue() const;

      public:
        // Non-virtual methods which give identity to the data just in terms of elementId and pathToValue.
        std::size_t getHash() const;

        inline friend bool operator==(const DataLocation& a, const DataLocation& b) { 
            return (a.m_elementId == b.m_elementId) && (a.m_pathToValue == b.m_pathToValue);
        }

        friend std::ostream& operator<<(std::ostream& os, const DataLocation& data) {
            return os << "\"" << data.m_pathToValue << " @ node " << data.m_elementId << "\"";
        }

      public:
        // Serialization.
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      private:
        ElementId m_elementId;
        Path m_pathToValue;
    };
}

namespace std {
    template <> struct hash<babelwires::DataLocation> {
        inline std::size_t operator()(const babelwires::DataLocation& data) const { return data.getHash(); }
    };
} // namespace std
