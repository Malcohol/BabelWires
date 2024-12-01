/**
 * TargetFileNodes are Nodes which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {
    class ValueTreeRoot;
    struct TargetFileNodeData;
    struct ProjectContext;

    /// TargetFileNodes are Nodes which correspond to a target file.
    class TargetFileNode : public FileNode {
      public:
        TargetFileNode(const ProjectContext& context, UserLogger& userLogger, const TargetFileNodeData& data,
                          NodeId newId);
        ~TargetFileNode();

        /// Down-cast version of the parent's method.
        const TargetFileNodeData& getNodeData() const;

        virtual const ValueTreeNode* getInput() const override;

        virtual std::filesystem::path getFilePath() const override;
        virtual void setFilePath(std::filesystem::path newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool save(const ProjectContext& context, UserLogger& userLogger) override;

        /// Adjusts the label to mark that the Node has not been saved since
        /// changes were made.
        virtual std::string getLabel() const override;

      protected:
        ValueTreeNode* doGetInputNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        void setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root);
        TargetFileNodeData& getNodeData();

        /// Should be called when either the contents or the file path change.
        void updateSaveHash();

      private:
        std::unique_ptr<ValueTreeRoot> m_valueTreeRoot;

        /// The current hash of the contents and filePath.
        std::size_t m_saveHash = 1;

        /// The hash of the contents and filePath when the file was last saved.
        std::size_t m_saveHashWhenSaved = 0;
    };

} // namespace babelwires
