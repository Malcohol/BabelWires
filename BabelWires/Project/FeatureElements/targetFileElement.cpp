/**
 * TargetFileElements are FeatureElements which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/targetFileElement.hpp"

#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/FileFormat/fileFeature.hpp"
#include "BabelWires/FileFormat/targetFileFormat.hpp"
#include "BabelWires/Project/FeatureElements/failedFeature.hpp"
#include "BabelWires/Project/FeatureElements/targetFileElementData.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/IO/outFileStream.hpp"
#include "Common/Log/userLogger.hpp"
#include "Common/Utilities/hash.hpp"

#include <fstream>

babelwires::TargetFileElement::TargetFileElement(const ProjectContext& context, UserLogger& userLogger,
                                                 const TargetFileElementData& data, ElementId newId)
    : FileElement(data, newId) {
    auto elementData = getElementData();
    try {
        setFactoryName(elementData.m_factoryIdentifier);
        const TargetFileFormat& factory =
            context.m_targetFileFormatReg.getRegisteredEntry(elementData.m_factoryIdentifier);
        setFactoryName(factory.getName());
        setFeature(factory.createNewFeature());
    } catch (const RegistryException& e) {
        setInternalFailure(e.what());
        // This is the wrong kind of identifier, but it shouldn't matter because this failure is unrecoverable.
        setFeature(std::make_unique<FailedFeature>());
        userLogger.logError() << "Failed to create target feature id=" << elementData.m_id << ": " << e.what();
    } catch (const BaseException& e) {
        setInternalFailure(e.what());
        // This is the wrong kind of identifier, but it shouldn't matter because this failure is unrecoverable.
        setFeature(std::make_unique<FileFeature>(elementData.m_factoryIdentifier));
        userLogger.logError() << "Failed to create target feature \"" << elementData.m_factoryIdentifier
                              << "\": " << e.what();
    }
}

const babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() const {
    return static_cast<const TargetFileElementData&>(FeatureElement::getElementData());
}

babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() {
    return static_cast<TargetFileElementData&>(FeatureElement::getElementData());
}

babelwires::RecordFeature* babelwires::TargetFileElement::getInputFeature() {
    return m_feature.get();
}

void babelwires::TargetFileElement::setFeature(std::unique_ptr<RecordFeature> feature) {
    m_contentsCache.setFeatures(feature.get(), nullptr);
    m_feature = std::move(feature);
}

std::filesystem::path babelwires::TargetFileElement::getFilePath() const {
    return getElementData().m_filePath;
}

void babelwires::TargetFileElement::setFilePath(std::filesystem::path newFilePath) {
    if (newFilePath != getElementData().m_filePath) {
        getElementData().m_filePath = std::move(newFilePath);
        setChanged(Changes::FileChanged);
        updateSaveHash();
    }
}

const babelwires::FileTypeEntry*
babelwires::TargetFileElement::getFileFormatInformation(const ProjectContext& context) const {
    // TODO: tryGetRegisteredEntry
    try {
        const TargetFileFormat& format =
            context.m_targetFileFormatReg.getRegisteredEntry(getElementData().m_factoryIdentifier);
        return &format;
    } catch (const RegistryException& e) {
    }
    return nullptr;
}

babelwires::FileElement::FileOperations babelwires::TargetFileElement::getSupportedFileOperations() const {
    return FileOperations::save;
}

bool babelwires::TargetFileElement::save(const ProjectContext& context, UserLogger& userLogger) {
    const auto& data = getElementData();
    if (isFailed()) {
        userLogger.logError() << "Cannot write output for failed TargetFileElement (id=" << data.m_id << ")";
        return false;
    }
    if (data.m_filePath.empty()) {
        userLogger.logError() << "Cannot write output for TargetFileElement when there is no file path (id="
                              << data.m_id << ")";
        return false;
    }
    try {
        OutFileStream outStream(data.m_filePath);
        const auto& fileFeature = dynamic_cast<const FileFeature&>(*m_feature.get());
        const TargetFileFormat* format = context.m_targetFileFormatReg.getEntryByIdentifier(data.m_factoryIdentifier);
        assert(format && "FileFeature with unregistered file format");
        format->writeToFile(fileFeature, outStream, userLogger);
        outStream.close();
        if (m_saveHashWhenSaved != m_saveHash) {
            setChanged(Changes::FeatureElementLabelChanged);
            m_saveHashWhenSaved = m_saveHash;
        }
        return true;
    } catch (const std::exception& e) {
        userLogger.logError() << "Failed to write output for TargetFileElement (id=" << data.m_id << "): " << e.what();
    }
    return false;
}

void babelwires::TargetFileElement::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        m_contentsCache.setFeatures(m_feature.get(), nullptr);
    } else if (isChanged(Changes::ModifierChangesMask)) {
        m_contentsCache.updateModifierCache();
    }

    if (m_feature->isChanged(Feature::Changes::SomethingChanged)) {
        updateSaveHash();
    }
}

std::string babelwires::TargetFileElement::getLabel() const {
    if (m_saveHash == m_saveHashWhenSaved) {
        return FeatureElement::getLabel();
    } else {
        return FeatureElement::getLabel() + "*";
    }
}

void babelwires::TargetFileElement::updateSaveHash() {
    std::size_t newHash = m_feature->getHash();
    hash::mixInto(newHash, getFilePath().u8string());

    if (m_saveHash != newHash) {
        if (m_saveHash == m_saveHashWhenSaved) {
            setChanged(Changes::FeatureElementLabelChanged);
        }
        m_saveHash = newHash;
    }
}
