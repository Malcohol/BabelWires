/**
 * SourceFileData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElementData.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {
    /// Describes the construction of a SourceFileFeature.
    struct SourceFileData : ElementData {
        CLONEABLE(SourceFileData);
        CUSTOM_CLONEABLE(SourceFileData);
        SERIALIZABLE(SourceFileData, "sourceFile", ElementData, 1);
        SourceFileData() = default;
        SourceFileData(const SourceFileData&) = default;
        SourceFileData(const SourceFileData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// The file containing the data.
        std::filesystem::path m_absoluteFilePath;

      protected:
        std::unique_ptr<FeatureElement> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;
    };
}
