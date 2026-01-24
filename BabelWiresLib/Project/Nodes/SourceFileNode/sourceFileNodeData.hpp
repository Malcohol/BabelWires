/**
 * SourceFileNodeData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BaseLib/DataContext/filePath.hpp>

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {
    /// Describes the construction of a SourceFileFeature.
    struct SourceFileNodeData : NodeData {
        CLONEABLE(SourceFileNodeData);
        CUSTOM_CLONEABLE(SourceFileNodeData);
        SERIALIZABLE(SourceFileNodeData, "sourceFile", NodeData, 1);
        SourceFileNodeData() = default;
        SourceFileNodeData(const SourceFileNodeData&) = default;
        SourceFileNodeData(const SourceFileNodeData& other, ShallowCloneContext);

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
