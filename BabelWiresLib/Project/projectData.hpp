/**
 * ProjectData carries data sufficient to reconstruct the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>

namespace babelwires {

    /// Carries data sufficient to reconstruct the project.
    struct ProjectData : Serializable, ProjectVisitable {
        SERIALIZABLE(ProjectData, "projectData", void, 1);
        ProjectData() = default;
        ProjectData(ProjectData&&) = default;
        ProjectData(const ProjectData& other);

        ProjectData& operator=(const ProjectData&) = delete;

        ProjectData& operator=(ProjectData&&) = default;

        // Serialization.
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// Call the visitor on all the FieldIdentifiers in the ProjectData.
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// Call the visitor on all the FilePaths in the ProjectData.
        /// This base implementation does nothing.
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// A randomly assigned ID which uniquely identifies this project.
        ProjectId m_projectId = INVALID_PROJECT_ID;

        std::vector<std::unique_ptr<NodeData>> m_nodes;
    };

} // namespace babelwires
