/**
 * SourceFileElements are FeatureElements which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct SourceFileElementData;
    class RecordFeature;
    struct ProjectContext;
    class RootFeature;

    /// SourceFileElements are FeatureElements which correspond to a source file.
    class SourceFileElement : public FileElement {
      public:
        SourceFileElement(const ProjectContext& context, UserLogger& userLogger, const SourceFileElementData& data,
                          ElementId newId);

        /// Down-cast version of the parent's method.
        const SourceFileElementData& getElementData() const;

        virtual const Feature* getOutputFeature() const override;
        
        virtual std::filesystem::path getFilePath() const override;
        virtual void setFilePath(std::filesystem::path newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool reload(const ProjectContext& context, UserLogger& userLogger) override;

      protected:
        Feature* doGetOutputFeatureNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        void setFeature(std::unique_ptr<RootFeature> feature);
        SourceFileElementData& getElementData();

      private:
        std::unique_ptr<RootFeature> m_feature;
    };

} // namespace babelwires
