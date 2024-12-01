/**
 * SourceFileNodes are Nodes which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct SourceFileNodeData;
    struct ProjectContext;
    class ValueTreeRoot;

    /// SourceFileNodes are Nodes which correspond to a source file.
    class SourceFileNode : public FileNode {
      public:
        SourceFileNode(const ProjectContext& context, UserLogger& userLogger, const SourceFileNodeData& data,
                          NodeId newId);

        /// Down-cast version of the parent's method.
        const SourceFileNodeData& getElementData() const;

        virtual const ValueTreeNode* getOutput() const override;
        
        virtual std::filesystem::path getFilePath() const override;
        virtual void setFilePath(std::filesystem::path newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool reload(const ProjectContext& context, UserLogger& userLogger) override;

      protected:
        ValueTreeNode* doGetOutputNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        void setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root);
        SourceFileNodeData& getElementData();

      private:
        std::unique_ptr<ValueTreeRoot> m_valueTreeRoot;
    };

} // namespace babelwires
