/**
 * SourceFileElements are FeatureElements which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct SourceFileElementData;
    struct ProjectContext;
    class ValueTreeRoot;

    /// SourceFileElements are FeatureElements which correspond to a source file.
    class SourceFileElement : public FileElement {
      public:
        SourceFileElement(const ProjectContext& context, UserLogger& userLogger, const SourceFileElementData& data,
                          ElementId newId);

        /// Down-cast version of the parent's method.
        const SourceFileElementData& getElementData() const;

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
        void setFeature(std::unique_ptr<ValueTreeRoot> feature);
        SourceFileElementData& getElementData();

      private:
        std::unique_ptr<ValueTreeRoot> m_valueTreeRoot;
    };

} // namespace babelwires
