/**
 * ProcessorData describe the construction of a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElementData.hpp"

namespace babelwires {
    /// Describes the construction of a processor.
    struct ProcessorData : ElementData {
        CLONEABLE(ProcessorData);
        CUSTOM_CLONEABLE(ProcessorData);
        SERIALIZABLE(ProcessorData, "processor", ElementData, 1);
        ProcessorData() = default;
        ProcessorData(const ProcessorData& other) = default;
        ProcessorData(const ProcessorData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      protected:
        std::unique_ptr<FeatureElement> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;
    };  
}
