/**
 * TargetFileElements are FeatureElements which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElement.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElementData.hpp>
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
    const NodeData& elementData = getElementData();
    try {
        setFactoryName(elementData.m_factoryIdentifier);
        const TargetFileFormat& factory =
            context.m_targetFileFormatReg.getRegisteredEntry(elementData.m_factoryIdentifier);
        setFactoryName(factory.getName());
        auto newFeature = factory.createNewFeature(context);
        newFeature->setToDefault();
        setValueTreeRoot(std::move(newFeature));
    } catch (const RegistryException& e) {
        setInternalFailure(e.what());
        setValueTreeRoot(std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisType()));
        userLogger.logError() << "Failed to create target id=" << elementData.m_id << ": " << e.what();
    } catch (const BaseException& e) {
        setInternalFailure(e.what());
        setValueTreeRoot(std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisType()));
        userLogger.logError() << "Failed to create target \"" << elementData.m_factoryIdentifier
                              << "\": " << e.what();
    }
}

babelwires::TargetFileElement::~TargetFileElement() = default;

const babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() const {
    return static_cast<const TargetFileElementData&>(Node::getElementData());
}

babelwires::TargetFileElementData& babelwires::TargetFileElement::getElementData() {
    return static_cast<TargetFileElementData&>(Node::getElementData());
}

babelwires::ValueTreeNode* babelwires::TargetFileElement::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::TargetFileElement::getInput() const {
    return m_valueTreeRoot.get();
}

void babelwires::TargetFileElement::setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root) {
    setValueTrees("File", root.get(), nullptr);
    m_valueTreeRoot = std::move(root);
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
        format->writeToFile(context, userLogger, *m_valueTreeRoot, outStream);
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
        setValueTrees("File", m_valueTreeRoot.get(), nullptr);
    } else if (isChanged(Changes::ModifierChangesMask)) {
        updateModifierCache();
    }

    if (m_valueTreeRoot->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
        updateSaveHash();
    }
}

std::string babelwires::TargetFileElement::getLabel() const {
    if (m_saveHash == m_saveHashWhenSaved) {
        return Node::getLabel();
    } else {
        return Node::getLabel() + "*";
    }
}

void babelwires::TargetFileElement::updateSaveHash() {
    std::size_t newHash = m_valueTreeRoot->getHash();
    hash::mixInto(newHash, getFilePath().u8string());

    if (m_saveHash != newHash) {
        if (m_saveHash == m_saveHashWhenSaved) {
            setChanged(Changes::FeatureElementLabelChanged);
        }
        m_saveHash = newHash;
    }
}