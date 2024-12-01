/**
 * TargetFileNodeData describes the construction of a TargetFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

#include <Common/DataContext/filePath.hpp>

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {
    /// Describes the construction of a TargetFileFeature.
    struct TargetFileNodeData : NodeData {
        CLONEABLE(TargetFileNodeData);
        CUSTOM_CLONEABLE(TargetFileNodeData);
        SERIALIZABLE(TargetFileNodeData, "targetFile", NodeData, 1);
        TargetFileNodeData() = default;
        TargetFileNodeData(const TargetFileNodeData& other) = default;
        TargetFileNodeData(const TargetFileNodeData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// The file containing the data.
        FilePath m_filePath;

      protected:
        std::unique_ptr<Node> doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                               NodeId newId) const override;
    };
}
