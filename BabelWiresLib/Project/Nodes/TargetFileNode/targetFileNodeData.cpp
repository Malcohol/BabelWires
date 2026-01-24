/**
 * TargetFileNodeData describes the construction of a TargetFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>

#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>

#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/exceptions.hpp>

babelwires::TargetFileNodeData::TargetFileNodeData(const TargetFileNodeData& other, ShallowCloneContext c)
    : NodeData(other, c)
    , m_filePath(other.m_filePath) {}

bool babelwires::TargetFileNodeData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_targetFileFormatReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::Node>
babelwires::TargetFileNodeData::doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                   NodeId newId) const {
    return std::make_unique<TargetFileNode>(context, userLogger, *this, newId);
}

void babelwires::TargetFileNodeData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeValue("filePath", m_filePath);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::TargetFileNodeData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializer.deserializeValue("filePath", m_filePath);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

void babelwires::TargetFileNodeData::visitFilePaths(FilePathVisitor& visitor) {
    NodeData::visitFilePaths(visitor);
    visitor(m_filePath);
}
