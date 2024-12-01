/**
 * ProcessorElementData describe the construction of a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/nodeData.hpp>

namespace babelwires {
    /// Describes the construction of a processor.
    struct ProcessorElementData : ElementData {
        CLONEABLE(ProcessorElementData);
        CUSTOM_CLONEABLE(ProcessorElementData);
        SERIALIZABLE(ProcessorElementData, "processor", ElementData, 1);
        ProcessorElementData() = default;
        ProcessorElementData(const ProcessorElementData& other) = default;
        ProcessorElementData(const ProcessorElementData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      protected:
        std::unique_ptr<Node> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;
    };  
}
