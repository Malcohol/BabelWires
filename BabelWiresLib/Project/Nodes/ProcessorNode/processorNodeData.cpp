/**
 * ProcessorNodeData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>

#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/exceptions.hpp>

babelwires::ProcessorNodeData::ProcessorNodeData(const ProcessorNodeData& other, ShallowCloneContext c)
    : NodeData(other, c) {}

bool babelwires::ProcessorNodeData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_processorReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::Node>
babelwires::ProcessorNodeData::doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                  NodeId newId) const {
    return std::make_unique<ProcessorNode>(context, userLogger, *this, newId);
}

void babelwires::ProcessorNodeData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

babelwires::Result babelwires::ProcessorNodeData::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(getCommonKeyValuePairs(deserializer));
    DO_OR_ERROR(deserializeModifiers(deserializer));
    DO_OR_ERROR(deserializeUiData(deserializer));
    return {};
}
