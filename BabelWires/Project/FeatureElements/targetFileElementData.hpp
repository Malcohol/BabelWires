/**
 * TargetFileElementData describes the construction of a TargetFileFeature.
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
    /// Describes the construction of a TargetFileFeature.
    struct TargetFileElementData : ElementData {
        CLONEABLE(TargetFileElementData);
        CUSTOM_CLONEABLE(TargetFileElementData);
        SERIALIZABLE(TargetFileElementData, "targetFile", ElementData, 1);
        TargetFileElementData() = default;
        TargetFileElementData(const TargetFileElementData& other) = default;
        TargetFileElementData(const TargetFileElementData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// The file to which the feature should be saved.
        std::filesystem::path m_absoluteFilePath;

      protected:
        std::unique_ptr<FeatureElement> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;
    };
}