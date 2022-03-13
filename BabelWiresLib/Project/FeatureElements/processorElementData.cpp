/**
 * ProcessorElementData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/processorElementData.hpp"

#include "BabelWiresLib/Processors/processor.hpp"
#include "BabelWiresLib/Processors/processorFactory.hpp"
#include "BabelWiresLib/Processors/processorFactoryRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::ProcessorElementData::ProcessorElementData(const ProcessorElementData& other, ShallowCloneContext c)
    : ElementData(other, c) {}

bool babelwires::ProcessorElementData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_processorReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::ProcessorElementData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                  ElementId newId) const {
    return std::make_unique<ProcessorElement>(context, userLogger, *this, newId);
}

void babelwires::ProcessorElementData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::ProcessorElementData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}
