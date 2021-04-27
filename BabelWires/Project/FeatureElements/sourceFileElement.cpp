/**
 * SourceFileElements are FeatureElements which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/sourceFileElement.hpp"

#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/FileFormat/fileFeature.hpp"
#include "BabelWires/FileFormat/fileFormat.hpp"
#include "BabelWires/Project/FeatureElements/failedFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/projectContext.hpp"

#include "Common/IO/fileDataSource.hpp"
#include "Common/Log/userLogger.hpp"

babelwires::SourceFileElement::SourceFileElement(const ProjectContext& context, UserLogger& userLogger,
                                                 const SourceFileData& data, ElementId newId)
    : FileElement(data, newId) {
    reload(context, userLogger);
}

const babelwires::SourceFileData& babelwires::SourceFileElement::getElementData() const {
    return static_cast<const SourceFileData&>(FeatureElement::getElementData());
}

babelwires::SourceFileData& babelwires::SourceFileElement::getElementData() {
    return static_cast<SourceFileData&>(FeatureElement::getElementData());
}

babelwires::RecordFeature* babelwires::SourceFileElement::getOutputFeature() {
    return m_feature.get();
}

void babelwires::SourceFileElement::setFeature(std::unique_ptr<RecordFeature> feature) {
    m_contentsCache.setFeatures(nullptr, feature.get());
    m_feature = std::move(feature);
}

void babelwires::SourceFileElement::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        m_contentsCache.setFeatures(nullptr, m_feature.get());
    }
}

std::string babelwires::SourceFileElement::getFilePath() const {
    return getElementData().m_filePath;
}

void babelwires::SourceFileElement::setFilePath(std::string newFilePath) {
    std::string& filePath = getElementData().m_filePath;
    if (filePath != newFilePath) {
        filePath = std::move(newFilePath);
        setChanged(Changes::FileChanged);
    }
}

const babelwires::FileTypeEntry*
babelwires::SourceFileElement::getFileFormatInformation(const ProjectContext& context) const {
    // TODO: tryGetRegisteredEntry
   try {
        const FileTypeEntry& format = context.m_sourceFileFormatReg.getRegisteredEntry(getElementData().m_factoryIdentifier);
        return &format;
    } catch (const RegistryException& e) {
    }
    return nullptr;
}

babelwires::FileElement::FileOperations babelwires::SourceFileElement::getSupportedFileOperations() const {
    return FileOperations::reload;
}

bool babelwires::SourceFileElement::reload(const ProjectContext& context, UserLogger& userLogger) {
    const SourceFileData& data = getElementData();

    try {
        const SourceFileFormat& format = context.m_sourceFileFormatReg.getRegisteredEntry(data.m_factoryIdentifier);
        setFactoryName(format.getName());

        if (data.m_filePath.empty()) {
            throw ModelException() << "No file name";
        }

        FileDataSource file(data.m_filePath.c_str());
        setFeature(format.loadFromFile(file, userLogger));
        clearInternalFailure();
        return true;
    } catch (const RegistryException& e) {
        userLogger.logError() << "Could not create Source File Feature id=" << data.m_id << ": " << e.what();
        setFactoryName(data.m_factoryIdentifier);
        setInternalFailure(e.what());
        // A dummy feature
        setFeature(std::make_unique<FailedFeature>());
    } catch (const BaseException& e) {
        userLogger.logError() << "Source File Feature id=" << data.m_id << " could not be loaded: " << e.what();
        setInternalFailure(e.what());
        // A dummy file feature which allows the user to change the file via the context menu.
        setFeature(std::make_unique<FileFeature>(data.m_factoryIdentifier));
    }
    return false;
}
