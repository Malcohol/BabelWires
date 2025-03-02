/**
 * A ProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

namespace babelwires {

    class ProjectDataLocation : public DataLocation {
      public:
        CLONEABLE(ProjectDataLocation);
        SERIALIZABLE(ProjectDataLocation, "projectLocation", void, 1);

        ProjectDataLocation(NodeId elementId, Path pathToValue);
        ProjectDataLocation(const ProjectDataLocation& other) = default;

        NodeId getNodeId() const;
        const Path& getPathToValue() const;

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
        ProjectDataLocation() = default;

      private:
        NodeId m_nodeId;
        Path m_pathToValue;
    };
}
