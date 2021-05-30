/**
 * SourceFileElementData describes the construction of a SourceFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/sourceFileElementData.hpp"

#include "BabelWires/FileFormat/sourceFileFormat.hpp"
#include "BabelWires/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWires/Project/projectContext.hpp"
#include "BabelWires/Features/recordFeature.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::SourceFileElementData::SourceFileElementData(const SourceFileElementData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_absoluteFilePath(other.m_absoluteFilePath) {}

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
    serializer.serializeValue("filename", m_absoluteFilePath.u8string());
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::SourceFileElementData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    std::string filePath;
    deserializer.deserializeValue("filename", filePath);
    m_absoluteFilePath = filePath;
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

void babelwires::SourceFileElementData::visitFilePaths(FilePathVisitor& visitor) {
    ElementData::visitFilePaths(visitor);
    visitor(m_filePath);
}
