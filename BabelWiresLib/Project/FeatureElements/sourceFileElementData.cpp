/**
 * SourceFileElementData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"

#include "BabelWiresLib/FileFormat/sourceFileFormat.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/Features/recordFeature.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::SourceFileElementData::SourceFileElementData(const SourceFileElementData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_filePath(other.m_filePath) {}

bool babelwires::SourceFileElementData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_sourceFileFormatReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::SourceFileElementData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                   ElementId newId) const {
    return std::make_unique<SourceFileElement>(context, userLogger, *this, newId);
}

void babelwires::SourceFileElementData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeValue("filePath", m_filePath);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::SourceFileElementData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializer.deserializeValue("filePath", m_filePath);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

void babelwires::SourceFileElementData::visitFilePaths(FilePathVisitor& visitor) {
    ElementData::visitFilePaths(visitor);
    visitor(m_filePath);
}
