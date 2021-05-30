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
    //serializer.serializeValue("filename", m_filePath.u8string());
    // Inline the filepath contents.
    m_filePath.serializeContents(serializer);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::SourceFileElementData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    // The filepath contents are inlined.
    m_filePath.deserializeContents(deserializer);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

void babelwires::SourceFileElementData::visitFilePaths(FilePathVisitor& visitor) {
    ElementData::visitFilePaths(visitor);
    visitor(m_filePath);
}
