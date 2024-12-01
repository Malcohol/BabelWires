/**
 * FileNode are Nodes which corresponds to a file (e.g. a source or target file).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <filesystem>

namespace babelwires {
    class FileTypeEntry;

    /// A base class for Nodes which represents the contents of a file.
    class FileNode : public Node {
      public:
        FileNode(const NodeData& data, NodeId newId);

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

    DEFINE_ENUM_FLAG_OPERATORS(FileNode::FileOperations);

} // namespace babelwires
