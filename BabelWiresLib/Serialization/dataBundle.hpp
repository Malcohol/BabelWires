/**
 * A DataBundle carries DATA along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/FileFormat/filePath.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Log/userLogger.hpp>

#include <filesystem>

namespace babelwires {
    /// A DataBundle carries data which is independent of the current system, and carries metadata sufficient
    /// to version it (i.e. load it into a system which has changed since the data was saved).
    /// Some virtual methods are provided so subclasses can carry additional metadata. To give control over the
    /// file structure, this additional metadata can optionally be serialized outside instead of inside the data
    /// payload.
    template <typename DATA> class DataBundle : public Serializable {
      public:
        SERIALIZABLE_ABSTRACT(DataBundle, "dataBundle", void);
        DataBundle() = default;
        DataBundle(const DataBundle&) = delete;
        DataBundle(DataBundle&&) = default;

        DataBundle& operator=(const DataBundle&) = delete;
        DataBundle& operator=(DataBundle&&) = default;

        /// Construct a bundle from data.
        /// The data is modified to make the bundle independent of the current system.
        DataBundle(std::filesystem::path pathToProjectFile, DATA&& data);

        /// Returns the contained data, modified so it corresponds the current system.
        /// This object is invalidated after calling this.
        DATA resolveAgainstCurrentContext(const ProjectContext& context, const std::filesystem::path& pathToProjectFile,
                                          UserLogger& userLogger) &&;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        const DATA& getData() const { return m_data; }
        DATA& getData() { return m_data; }

        const IdentifierRegistry& getIdentifierRegistry() const { return m_identifierRegistry; }

      private:
        /// Ensure the fields in the data refer to the context independent m_identifierRegistry.
        void interpretIdentifiersInCurrentContext();

        /// Ensure the filePaths in the data are made relative to the m_projectFilePath.
        void interpretFilePathsInCurrentProjectPath();

        /// Ensure the fields in the data refer to the global FileNameRegistry.
        void resolveIdentifiersAgainstCurrentContext();

        /// Update the filePaths in the data, in terms of the given pathToProjectFile.
        void resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToProjectFile,
                                                       UserLogger& userLogger);

      protected:
        /// If the subclass needs to do any additional resolution, it can do it here.
        virtual void adaptDataToAdditionalMetadata(const ProjectContext& context, UserLogger& userLogger) {}

        /// Allows the subclass to put additional metadata at the same level as the metadata handled by this class.
        virtual void serializeAdditionalMetadata(Serializer& serializer) const {}

        /// Allows the subclass to put additional metadata at the same level as the metadata handled by this class.
        virtual void deserializeAdditionalMetadata(Deserializer& deserializer) {}

      private:
        /// The data.
        DATA m_data;

        /// Identifier metadata.
        IdentifierRegistry m_identifierRegistry;

        /// Absolute path to the project, when saved.
        /// FilePaths in the data are stored in relative form whenever possible.
        /// Having this allows us to reconstruct their absolute paths in case the relative path
        /// from the project as loaded does not exist.
        FilePath m_projectFilePath;
    };

} // namespace babelwires

#include <BabelWiresLib/Serialization/dataBundle_inl.hpp>
