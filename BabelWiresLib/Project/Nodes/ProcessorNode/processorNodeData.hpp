/**
 * ProcessorNodeData describe the construction of a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

namespace babelwires {
    /// Describes the construction of a processor.
    struct ProcessorNodeData : NodeData {
        CLONEABLE(ProcessorNodeData);
        CUSTOM_CLONEABLE(ProcessorNodeData);
        SERIALIZABLE(ProcessorNodeData, "processor", NodeData, 1);
        ProcessorNodeData() = default;
        ProcessorNodeData(const ProcessorNodeData& other) = default;
        ProcessorNodeData(const ProcessorNodeData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      protected:
        std::unique_ptr<Node> doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                               NodeId newId) const override;
    };  
}
