/**
 * A DataBundle carries DATA along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/DataContext/filePath.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Log/userLogger.hpp>
#include <Common/Serialization/serializable.hpp>

#include <filesystem>

namespace babelwires {
    /// A DataBundle carries data which is independent of the current system, and carries metadata sufficient
    /// to load it into a system which may not be identical to the system in which it was saved.
    /// This base class handles metadata for identifiers and paths, although the subclass must implement the
    /// ProjectVisitable interface so the data (and possibly additional metadata) get visited.
    /// Some virtual methods are provided so subclasses can provide support for additional metadata.
    template <typename DATA> class DataBundle : public Serializable, public ProjectVisitable {
      public:
        using Data = DATA;

        SERIALIZABLE_ABSTRACT(DataBundle, void);
        DataBundle() = default;
        DataBundle(const DataBundle&) = delete;
        DataBundle(DataBundle&&) = default;

        DataBundle& operator=(const DataBundle&) = delete;
        DataBundle& operator=(DataBundle&&) = default;

        /// Construct a bundle from data.
        /// This object takes ownership of the data.
        DataBundle(std::filesystem::path pathToFile, DATA&& data);

        /// Should be called before saving to make the bundle independent of the current system
        // TODO Should probably take the context as a parameter.
        void interpretInCurrentContext();

        /// Returns the contained data, modified so it corresponds the current system.
        /// This object is invalidated after calling this.
        DATA resolveAgainstCurrentContext(const ProjectContext& context, const std::filesystem::path& pathToFile,
                                          UserLogger& userLogger) &&;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        const DATA& getData() const { return m_data; }
        DATA& getData() { return m_data; }

        const IdentifierRegistry& getIdentifierRegistry() const { return m_identifierRegistry; }

      protected:
        /// Gives the subclass the opportunity to populate the additional metadata, or at least
        /// make it independent of the current system.
        virtual void interpretAdditionalMetadataInCurrentContext() {}

        /// If the subclass needs to do any additional resolution, it can do it here.
        virtual void adaptDataToAdditionalMetadata(const ProjectContext& context, UserLogger& userLogger) {}

        /// Allows the subclass to put additional metadata at the same level as the metadata handled by this class.
        virtual void serializeAdditionalMetadata(Serializer& serializer) const {}

        /// Allows the subclass to put additional metadata at the same level as the metadata handled by this class.
        virtual void deserializeAdditionalMetadata(Deserializer& deserializer) {}

      private:
        /// Ensure the identifiers in the data refer to the context independent m_identifierRegistry.
        void interpretIdentifiersInCurrentContext();

        /// Ensure the filePaths in the data are made relative to the m_pathToFile.
        void interpretFilePathsInCurrentProjectPath();

        /// Ensure the identifiers in the data refer to the global IdentifierRegistry.
        void resolveIdentifiersAgainstCurrentContext();

        /// Update the filePaths in the data, in terms of the given pathToProjectFile.
        void resolveFilePathsAgainstCurrentProjectPath(const std::filesystem::path& pathToFile,
                                                       UserLogger& userLogger);

        /// Convert identifiers, relative to the sourceReg, to identifiers, relative to the targetReg.
        template <typename SOURCE_REG, typename TARGET_REG>
        void convertIdentifiers(SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                         babelwires::IdentifierRegistry::Authority authority);

      private:
        /// The data payload.
        DATA m_data;

        /// Identifier metadata.
        IdentifierRegistry m_identifierRegistry;

        /// Absolute path to the file when saved.
        /// FilePaths in the data are stored in relative form whenever possible.
        /// Having this allows us to reconstruct their absolute paths in case the relative path
        /// from the file as loaded does not exist.
        FilePath m_pathToFile;
    };

} // namespace babelwires

#include <BabelWiresLib/Serialization/dataBundle_inl.hpp>
