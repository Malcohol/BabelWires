/**
 * FeatureElementDatas carry the data sufficient to reconstruct a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/FileFormat/sourceFileFormat.hpp"
#include "BabelWires/FileFormat/targetFileFormat.hpp"
#include "BabelWires/Processors/processor.hpp"
#include "BabelWires/Processors/processorFactory.hpp"
#include "BabelWires/Processors/processorFactoryRegistry.hpp"
#include "BabelWires/Project/FeatureElements/processorElement.hpp"
#include "BabelWires/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWires/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

#include <cassert>

void babelwires::UiData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("x", m_uiPosition.m_x);
    serializer.serializeValue("y", m_uiPosition.m_y);
    serializer.serializeValue("width", m_uiSize.m_width);
}

void babelwires::UiData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("x", m_uiPosition.m_x);
    deserializer.deserializeValue("y", m_uiPosition.m_y);
    deserializer.deserializeValue("width", m_uiSize.m_width);
}

babelwires::ElementData::ElementData(const ElementData& other, ShallowCloneContext)
    : Cloneable(other)
    , m_factoryIdentifier(other.m_factoryIdentifier)
    , m_factoryVersion(other.m_factoryVersion)
    , m_id(other.m_id)
    , m_uiData(other.m_uiData) {}

babelwires::ElementData::ElementData(const ElementData& other)
    : ElementData(other, ShallowCloneContext()) {
    for (auto&& m : other.m_modifiers) {
        m_modifiers.push_back(m->clone());
    }
    m_expandedPaths = other.m_expandedPaths;
}

std::unique_ptr<babelwires::FeatureElement> babelwires::ElementData::createFeatureElement(const ProjectContext& context,
                                                                                          UserLogger& userLogger,
                                                                                          ElementId newId) const {
    std::unique_ptr<babelwires::FeatureElement> newElement = doCreateFeatureElement(context, userLogger, newId);
    newElement->applyLocalModifiers(userLogger);
    return newElement;
}

void babelwires::ElementData::addCommonKeyValuePairs(Serializer& serializer) const {
    serializer.serializeValue("id", m_id);
    serializer.serializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::ElementData::getCommonKeyValuePairs(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_id);
    deserializer.deserializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::ElementData::serializeModifiers(Serializer& serializer) const {
    serializer.serializeArray("modifiers", m_modifiers);
}

void babelwires::ElementData::deserializeModifiers(Deserializer& deserializer) {
    for (auto it = deserializer.deserializeArray<ModifierData>("modifiers", Deserializer::IsOptional::Optional);
         it.isValid(); ++it) {
        m_modifiers.emplace_back(it.getObject());
    }
}

void babelwires::ElementData::serializeUiData(Serializer& serializer) const {
    serializer.serializeObject(m_uiData);
    serializer.serializeValueArray("expandedPaths", m_expandedPaths, "path");
}

void babelwires::ElementData::deserializeUiData(Deserializer& deserializer) {
    if (auto uiData =
            deserializer.deserializeObject<UiData>(UiData::serializationType, Deserializer::IsOptional::Optional)) {
        m_uiData = *uiData;
    }
    for (auto it = deserializer.deserializeValueArray<FeaturePath>("expandedPaths", Deserializer::IsOptional::Optional,
                                                                   "path");
         it.isValid(); ++it) {
        m_expandedPaths.emplace_back(std::move(it.deserializeValue()));
    }
}

namespace {
    template <typename Registry>
    bool checkFactoryVersionCommon(const Registry& reg, babelwires::UserLogger& userLogger,
                                   const std::string factoryIdentifier, babelwires::VersionNumber& thisVersion) {
        if (const auto* entry = reg.getEntryByIdentifier(factoryIdentifier)) {
            const babelwires::VersionNumber registeredVersion = entry->getVersion();
            if (thisVersion == 0) {
                // Implicitly consider this version up-to-date.
                thisVersion = registeredVersion;
                return true;
            } else if (thisVersion < registeredVersion) {
                userLogger.logWarning() << "Data for the factory \"" << entry->getName() << "\" (" << factoryIdentifier
                                        << ") corresponds to an old version (" << thisVersion
                                        << "). The current version is " << registeredVersion
                                        << ". Some edits may not apply correctly.";
                return false;
            } else if (thisVersion > registeredVersion) {
                userLogger.logError() << "Data for the factory \"" << entry->getName() << "\" (" << factoryIdentifier
                                      << ") has an unknown version (" << thisVersion << "). The current version is "
                                      << registeredVersion << ". Some edits may not apply correctly.";
                return false;
            }
            return true;
        }
        return true;
    }
} // namespace

babelwires::SourceFileData::SourceFileData(const SourceFileData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_filePath(other.m_filePath) {}

bool babelwires::SourceFileData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_sourceFileFormatReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::SourceFileData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                   ElementId newId) const {
    return std::make_unique<SourceFileElement>(context, userLogger, *this, newId);
}

void babelwires::SourceFileData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeValue("filename", m_filePath);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::SourceFileData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializer.deserializeValue("filename", m_filePath);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

babelwires::TargetFileData::TargetFileData(const TargetFileData& other, ShallowCloneContext c)
    : ElementData(other, c)
    , m_filePath(other.m_filePath) {}

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
    serializer.serializeValue("filename", m_filePath);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::TargetFileData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializer.deserializeValue("filename", m_filePath);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

babelwires::ProcessorData::ProcessorData(const ProcessorData& other, ShallowCloneContext c)
    : ElementData(other, c) {}

bool babelwires::ProcessorData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    return checkFactoryVersionCommon(context.m_processorReg, userLogger, m_factoryIdentifier, m_factoryVersion);
}

std::unique_ptr<babelwires::FeatureElement>
babelwires::ProcessorData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                  ElementId newId) const {
    return std::make_unique<ProcessorElement>(context, userLogger, *this, newId);
}

void babelwires::ProcessorData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::ProcessorData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}
