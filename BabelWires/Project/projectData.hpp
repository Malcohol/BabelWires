/**
 * ProjectData carries data sufficient to reconstruct the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElementData.hpp"

#include "Common/Serialization/serializable.hpp"

namespace babelwires {

    /// Carries data sufficient to reconstruct the project.
    struct ProjectData : Serializable, ProjectVisitable {
        SERIALIZABLE(ProjectData, "projectData", void, 1);
        ProjectData() = default;
        ProjectData(ProjectData&&) = default;

        /// Deleted, to stop ProjectData being unnecessarily copied.
        /// If required, the m_elements will need to be cloned.
        ProjectData(const ProjectData& other) = delete;

        ProjectData& operator=(const ProjectData&) = delete;

        ProjectData& operator=(ProjectData&&) = default;

        // Serialization.
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// Call the visitor on all the FieldIdentifiers in the element.
        void visitFields(FieldVisitor& visitor) override;

        /// Call the visitor on all the FilePaths in the element.
        /// This base implementation does nothing.
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// A randomly assigned ID which uniquely identifies this project.
        ProjectId m_projectId = INVALID_PROJECT_ID;

        std::vector<std::unique_ptr<ElementData>> m_elements;
    };

} // namespace babelwires
