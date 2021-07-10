/**
 * SourceFileElementData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/FileFormat/filePath.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {
    /// Describes the construction of a SourceFileFeature.
    struct SourceFileElementData : ElementData {
        CLONEABLE(SourceFileElementData);
        CUSTOM_CLONEABLE(SourceFileElementData);
        SERIALIZABLE(SourceFileElementData, "sourceFile", ElementData, 1);
        SourceFileElementData() = default;
        SourceFileElementData(const SourceFileElementData&) = default;
        SourceFileElementData(const SourceFileElementData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// The file containing the data.
        FilePath m_filePath;

      protected:
        std::unique_ptr<FeatureElement> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;
    };
}