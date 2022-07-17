/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Serialization/dataBundle.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

#include <map>

namespace babelwires {
    /// A ProjectBundle carries ProjectData which is independent of the current system, and carries metadata sufficient
    /// to version it (i.e. load it into a system which has changed since the data was saved).
    class ProjectBundle : public DataBundle<ProjectData> {
      public:
        SERIALIZABLE(ProjectBundle, "project", void, 1);
        ProjectBundle() = default;
        ProjectBundle(ProjectBundle&&) = default;
        ProjectBundle& operator=(ProjectBundle&&) = default;

        /// Construct a bundle from projectData.
        /// The projectData is modified to make the bundle independent of the current system.
        ProjectBundle(std::filesystem::path pathToProjectFile, ProjectData&& projectData);

      public:
        /// Information about the factories used by the projectData.
        using FactoryMetadata = std::map<LongIdentifier, VersionNumber>;

        const FactoryMetadata& getFactoryMetadata() const { return m_factoryMetadata; }

      protected:
        /// Populate the factoryMetadata.
        void interpretAdditionalMetadataInCurrentContext() override;

        /// The versions of the factories in the stored ProjectData from the factory meta-data are updated.
        /// NOTE: Right now, no versioning is done! All that happens is that warnings and errors are issued.
        void adaptDataToAdditionalMetadata(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeAdditionalMetadata(Serializer& serializer) const override;
        void deserializeAdditionalMetadata(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      private:
        /// Information about the factories.
        FactoryMetadata m_factoryMetadata;
    };

} // namespace babelwires
