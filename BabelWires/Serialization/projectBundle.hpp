/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/FileFormat/filePath.hpp"
#include "BabelWires/Project/projectData.hpp"

#include "Common/Serialization/serializable.hpp"

#include <map>

namespace babelwires {
    /// A ProjectBundle carries ProjectData which is independent of the current system, and carries metadata sufficient
    /// to version it (i.e. load it into a system which has changed since the data was saved).
    class ProjectBundle : public Serializable {
      public:
        SERIALIZABLE(ProjectBundle, "project", void, 1);
        ProjectBundle();
        ProjectBundle(const ProjectBundle&) = delete;
        ProjectBundle(ProjectBundle&&) = default;

        ProjectBundle& operator=(const ProjectBundle&) = delete;
        ProjectBundle& operator=(ProjectBundle&&) = default;

        /// Construct a bundle from projectData.
        /// The projectData is modified to make the bundle independent of the current system.
        ProjectBundle(std::filesystem::path pathToProjectFile, ProjectData&& projectData);

        /// Returns the contained projectData, modified so it corresponds the current system.
        /// This object is invalidated after calling this.
        /// The versions of the factories in the stored ProjectData from the factory meta-data are updated.
        /// NOTE: No versioning is done! All that currently happens is that warnings and errors are issued.
        ProjectData resolveAgainstCurrentContext(const ProjectContext& context, const std::filesystem::path& pathToProjectFile,
                                                       UserLogger& userLogger) &&;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      private:
        /// Ensure the fields in the projectData refer to the context independent m_fieldNameRegistry.
        void interpretFieldsInCurrentContext();

        /// Ensure the filePaths in the projectData are made relative to the m_projectFilePath.
        void interpretFilePathsInCurrentProjectPath();

        /// Ensure the fields in the projectData refer to the global FileNameRegistry.
        void resolveFieldsAgainstCurrentContext();

        /// Update the filePaths in the projectData, in terms of the given pathToProjectFile.
        void resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToProjectFile,
                                                       UserLogger& userLogger);

      public:
        // Used by tests.
        const FieldNameRegistry& getFieldNameRegistry() const { return m_fieldNameRegistry; }

        const ProjectData& getProjectData() const { return m_projectData; }

        struct Metadata {
            /// Factory meta-data
            std::map<std::string, VersionNumber> m_factoryMetadata;
        };

        const Metadata& getMetadata() const { return m_metadata; }

      private:
        /// The data.
        ProjectData m_projectData;

        /// Field meta-data.
        FieldNameRegistry m_fieldNameRegistry;

        Metadata m_metadata;

        /// Absolute path to the project, when saved.
        /// FilePaths in the projectData are stored in relative form whenever possible.
        /// Having this allows us to reconstruct their absolute paths in case the relative path
        /// from the project as loaded does not exist.
        FilePath m_projectFilePath;
    };

} // namespace babelwires
