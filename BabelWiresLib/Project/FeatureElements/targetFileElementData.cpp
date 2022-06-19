/**
 * TargetFileElementData describes the construction of a TargetFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"

#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/FileFormat/targetFileFormat.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::TargetFileElementData::TargetFileElementData(const TargetFileElementData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_filePath(other.m_filePath) {}

bool babelwires::TargetFileElementData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_targetFileFormatReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::TargetFileElementData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                   ElementId newId) const {
    return std::make_unique<TargetFileElement>(context, userLogger, *this, newId);
}

void babelwires::TargetFileElementData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeValue("filePath", m_filePath);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

babelwires::TargetFileElementData::TargetFileElementData(Deserializer& deserializer) : ElementData(deserializer) {
    deserializer.deserializeValue("filePath", m_filePath);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

void babelwires::TargetFileElementData::visitFilePaths(FilePathVisitor& visitor) {
    ElementData::visitFilePaths(visitor);
    visitor(m_filePath);
}
