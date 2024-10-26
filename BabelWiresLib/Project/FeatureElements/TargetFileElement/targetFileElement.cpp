/**
 * TargetFileElements are FeatureElements which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElement.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/valueTreeRoot.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/IO/outFileStream.hpp>
#include <Common/Log/userLogger.hpp>

#include <fstream>

babelwires::TargetFileElement::TargetFileElement(const ProjectContext& context, UserLogger& userLogger,
                                                 const TargetFileElementData& data, ElementId newId)
    : FileElement(data, newId) {
    const ElementData& elementData = getElementData();
    try {
        setFactoryName(elementData.m_factoryIdentifier);
        const TargetFileFormat& factory =
            context.m_targetFileFormatReg.getRegisteredEntry(elementData.m_factoryIdentifier);
        setFactoryName(factory.getName());
        auto newFeature = factory.createNewFeature(context);
        newFeature->setToDefault();
        setFeature(std::move(newFeature));
    } catch (const RegistryException& e) {
        setInternalFailure(e.what());
        setFeature(std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisIdentifier()));
        userLogger.logError() << "Failed to create target feature id=" << elementData.m_id << ": " << e.what();
    } catch (const BaseException& e) {
        setInternalFailure(e.what());
        setFeature(std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisIdentifier()));
        userLogger.logError() << "Failed to create target feature \"" << elementData.m_factoryIdentifier
                              << "\": " << e.what();
    }
}

babelwires::TargetFileElement::~TargetFileElement() = default;

const babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() const {
    return static_cast<const TargetFileElementData&>(FeatureElement::getElementData());
}

babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() {
    return static_cast<TargetFileElementData&>(FeatureElement::getElementData());
}

babelwires::ValueTreeNode* babelwires::TargetFileElement::doGetInputFeatureNonConst() {
    return m_feature.get();
}

const babelwires::ValueTreeNode* babelwires::TargetFileElement::getInputFeature() const {
    return m_feature.get();
}

void babelwires::TargetFileElement::setFeature(std::unique_ptr<ValueTreeRoot> feature) {
    m_contentsCache.setFeatures("File", feature.get(), nullptr);
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
    } catch (const RegistryException&) {
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
        const TargetFileFormat* format = context.m_targetFileFormatReg.getEntryByIdentifier(data.m_factoryIdentifier);
        assert(format && "FileFeature with unregistered file format");
        format->writeToFile(context, userLogger, *m_feature, outStream);
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
        m_contentsCache.setFeatures("File", m_feature.get(), nullptr);
    } else if (isChanged(Changes::ModifierChangesMask)) {
        m_contentsCache.updateModifierCache();
    }

    if (m_feature->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
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
