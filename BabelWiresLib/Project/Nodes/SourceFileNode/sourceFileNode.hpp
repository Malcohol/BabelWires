/**
 * SourceFileNodes are Nodes which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

namespace babelwires {
    struct UserLogger;
    struct SourceFileNodeData;
    struct Context;
    class ValueTreeRoot;

    /// SourceFileNodes are Nodes which correspond to a source file.
    class BABELWIRESLIB_API SourceFileNode : public FileNode {
      public:
        DOWNCASTABLE(SourceFileNode, FileNode);
        SourceFileNode(const Context& context, UserLogger& userLogger, const SourceFileNodeData& data,
                          NodeId newId);

        /// Down-cast version of the parent's method.
        const SourceFileNodeData& getNodeData() const;

        virtual const ValueTreeNode* getOutput() const override;
        
        virtual std::filesystem::path getFilePath() const override;
        virtual void setFilePath(std::filesystem::path newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const Context& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool reload(const Context& context, UserLogger& userLogger) override;

      protected:
        ValueTreeNode* doGetOutputNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        void setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root);
        SourceFileNodeData& getNodeData();

      private:
        std::unique_ptr<ValueTreeRoot> m_valueTreeRoot;
    };

} // namespace babelwires
