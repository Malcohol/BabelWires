/**
 * TargetFileData describes the construction of a TargetFileFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/targetFileElementData.hpp"

#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/FileFormat/targetFileFormat.hpp"
#include "BabelWires/Project/projectContext.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/targetFileElement.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::TargetFileData::TargetFileData(const TargetFileData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_absoluteFilePath(other.m_absoluteFilePath) {}

bool babelwires::TargetFileData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_targetFileFormatReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::TargetFileData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                   ElementId newId) const {
    return std::make_unique<TargetFileElement>(context, userLogger, *this, newId);
}

void babelwires::TargetFileData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeValue("filename", m_absoluteFilePath.u8string());
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::TargetFileData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    std::string path;
    deserializer.deserializeValue("filename", path);
    m_absoluteFilePath = path;
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}
