/**
 * FileElement are FeatureElements which corresponds to a file (e.g. a source or target file).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElement.hpp"

#include <filesystem>

namespace babelwires {
    class FileTypeEntry;

    /// A feature element which represents the contents of a file.
    class FileElement : public FeatureElement {
      public:
        FileElement(const ElementData& data, ElementId newId);

        /// Return the file path to the file.
        virtual std::filesystem::path getFilePath() const = 0;

        /// Set the file path to the file.
        /// Implementations are not expected to perform file operations.
        virtual void setFilePath(std::filesystem::path newFilePath) = 0;

        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const = 0;

        enum class FileOperations : unsigned int { reload = 0b01, save = 0b10 };

        virtual FileOperations getSupportedFileOperations() const = 0;

        /// Attempt to reload the contents from disk and return true if the operation was successful.
        /// The default implementation asserts.
        virtual bool reload(const ProjectContext& context, UserLogger& userLogger);

        /// Attempt to write the contents to disk and return true if the operation was successful.
        /// The default implementation asserts.
        virtual bool save(const ProjectContext& context, UserLogger& userLogger);
    };

    DEFINE_ENUM_FLAG_OPERATORS(FileElement::FileOperations);

} // namespace babelwires
