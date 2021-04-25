/**
 * SourceFileElements are FeatureElements which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/fileElement.hpp"

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct SourceFileData;
    class RecordFeature;
    class ProjectContext;

    /// SourceFileElements are FeatureElements which correspond to a source file.
    class SourceFileElement : public FileElement {
      public:
        SourceFileElement(const ProjectContext& context, UserLogger& userLogger, const SourceFileData& data,
                          ElementId newId);

        /// Down-cast version of the parent's method.
        const SourceFileData& getElementData() const;

        virtual RecordFeature* getOutputFeature() override;
        using FileElement::getOutputFeature;

        virtual std::string getFilePath() const override;
        virtual void setFilePath(std::string newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool reload(const ProjectContext& context, UserLogger& userLogger) override;

      protected:
        void setFeature(std::unique_ptr<RecordFeature> feature);
        virtual void doProcess(UserLogger& userLogger) override;
        SourceFileData& getElementData();

      private:
        std::unique_ptr<RecordFeature> m_feature;
    };

} // namespace babelwires
