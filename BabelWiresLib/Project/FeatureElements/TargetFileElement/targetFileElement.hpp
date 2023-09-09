/**
 * TargetFileElements are FeatureElements which correspond to a target file.
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

    struct TargetFileElementData;
    class RecordFeature;
    struct ProjectContext;
    class FileFeature;

    /// TargetFileElements are FeatureElements which correspond to a target file.
    class TargetFileElement : public FileElement {
      public:
        TargetFileElement(const ProjectContext& context, UserLogger& userLogger, const TargetFileElementData& data,
                          ElementId newId);

        /// Down-cast version of the parent's method.
        const TargetFileElementData& getElementData() const;

        virtual const RootFeature* getInputFeature() const override;

        virtual std::filesystem::path getFilePath() const override;
        virtual void setFilePath(std::filesystem::path newFilePath) override;
        virtual const FileTypeEntry* getFileFormatInformation(const ProjectContext& context) const override;
        virtual FileOperations getSupportedFileOperations() const override;
        virtual bool save(const ProjectContext& context, UserLogger& userLogger) override;

        /// Adjusts the label to mark that the element has not been saved since
        /// changes were made.
        virtual std::string getLabel() const override;

      protected:
        RootFeature* doGetInputFeatureNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        void setFeature(std::unique_ptr<RootFeature> feature);
        TargetFileElementData& getElementData();

        /// Should be called when either the feature or the file path change.
        void updateSaveHash();

      private:
        std::unique_ptr<RootFeature> m_feature;

        /// The current hash of the feature and filePath.
        std::size_t m_saveHash = 1;

        /// The hash of the feature and filePath when the file was last saved.
        std::size_t m_saveHashWhenSaved = 0;
    };

} // namespace babelwires
