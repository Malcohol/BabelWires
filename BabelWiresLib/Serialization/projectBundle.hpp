/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/FileFormat/filePath.hpp"
#include "BabelWiresLib/Project/projectData.hpp"

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
        ProjectData resolveAgainstCurrentContext(const ProjectContext& context, const std::filesystem::path& pathToProjectFile,
                                                       UserLogger& userLogger) &&;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      private:
        /// Ensure the fields in the projectData refer to the context independent m_identifierRegistry.
        void interpretIdentifiersInCurrentContext();

        /// Ensure the filePaths in the projectData are made relative to the m_projectFilePath.
        void interpretFilePathsInCurrentProjectPath();

        /// Record the metadata of factories used by the projectData.
        void captureCurrentFactoryMetadata();

        /// Ensure the fields in the projectData refer to the global FileNameRegistry.
        void resolveIdentifiersAgainstCurrentContext();

        /// Update the filePaths in the projectData, in terms of the given pathToProjectFile.
        void resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToProjectFile,
                                                       UserLogger& userLogger);

        /// The versions of the factories in the stored ProjectData from the factory meta-data are updated.
        /// NOTE: Right now, no versioning is done! All that happens is that warnings and errors are issued.
        void adaptDataToCurrentFactories(const ProjectContext& context, UserLogger& userLogger);

      public:
        // Used by tests.

        const IdentifierRegistry& getIdentifierRegistry() const { return m_identifierRegistry; }

        const ProjectData& getProjectData() const { return m_projectData; }

        /// Information about the factories used by the projectData.
        using FactoryMetadata = std::map<Identifier, VersionNumber>;

        const FactoryMetadata& getFactoryMetadata() const { return m_factoryMetadata; }

      private:
        /// The data.
        ProjectData m_projectData;

        /// Information about the factories.
        FactoryMetadata m_factoryMetadata;

        /// Identifier metadata.
        IdentifierRegistry m_identifierRegistry;

        /// Absolute path to the project, when saved.
        /// FilePaths in the projectData are stored in relative form whenever possible.
        /// Having this allows us to reconstruct their absolute paths in case the relative path
        /// from the project as loaded does not exist.
        FilePath m_projectFilePath;
    };

} // namespace babelwires
