/**
 * TargetFileNodes are Nodes which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/IO/outFileStream.hpp>
#include <Common/Log/userLogger.hpp>

#include <fstream>

babelwires::TargetFileNode::TargetFileNode(const ProjectContext& context, UserLogger& userLogger,
                                                 const TargetFileNodeData& data, ElementId newId)
    : FileNode(data, newId) {
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

babelwires::TargetFileNode::~TargetFileNode() = default;

const babelwires::TargetFileNodeData& babelwires::TargetFileNode::getElementData() const {
    return static_cast<const TargetFileNodeData&>(Node::getElementData());
}

babelwires::TargetFileNodeData& babelwires::TargetFileNode::getElementData() {
    return static_cast<TargetFileNodeData&>(Node::getElementData());
}

babelwires::ValueTreeNode* babelwires::TargetFileNode::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::TargetFileNode::getInput() const {
    return m_valueTreeRoot.get();
}

void babelwires::TargetFileNode::setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root) {
    setValueTrees("File", root.get(), nullptr);
    m_valueTreeRoot = std::move(root);
}

std::filesystem::path babelwires::TargetFileNode::getFilePath() const {
    return getElementData().m_filePath;
}

void babelwires::TargetFileNode::setFilePath(std::filesystem::path newFilePath) {
    if (newFilePath != getElementData().m_filePath) {
        getElementData().m_filePath = std::move(newFilePath);
        setChanged(Changes::FileChanged);
        updateSaveHash();
    }
}

const babelwires::FileTypeEntry*
babelwires::TargetFileNode::getFileFormatInformation(const ProjectContext& context) const {
    // TODO: tryGetRegisteredEntry
    try {
        const TargetFileFormat& format =
            context.m_targetFileFormatReg.getRegisteredEntry(getElementData().m_factoryIdentifier);
        return &format;
    } catch (const RegistryException&) {
    }
    return nullptr;
}

babelwires::FileNode::FileOperations babelwires::TargetFileNode::getSupportedFileOperations() const {
    return FileOperations::save;
}

bool babelwires::TargetFileNode::save(const ProjectContext& context, UserLogger& userLogger) {
    const auto& data = getElementData();
    if (isFailed()) {
        userLogger.logError() << "Cannot write output for failed TargetFileNode (id=" << data.m_id << ")";
        return false;
    }
    if (data.m_filePath.empty()) {
        userLogger.logError() << "Cannot write output for TargetFileNode when there is no file path (id="
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
            setChanged(Changes::NodeLabelChanged);
            m_saveHashWhenSaved = m_saveHash;
        }
        return true;
    } catch (const std::exception& e) {
        userLogger.logError() << "Failed to write output for TargetFileNode (id=" << data.m_id << "): " << e.what();
    }
    return false;
}

void babelwires::TargetFileNode::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        setValueTrees("File", m_valueTreeRoot.get(), nullptr);
    } else if (isChanged(Changes::ModifierChangesMask)) {
        updateModifierCache();
    }

    if (m_valueTreeRoot->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
        updateSaveHash();
    }
}

std::string babelwires::TargetFileNode::getLabel() const {
    if (m_saveHash == m_saveHashWhenSaved) {
        return Node::getLabel();
    } else {
        return Node::getLabel() + "*";
    }
}

void babelwires::TargetFileNode::updateSaveHash() {
    std::size_t newHash = m_valueTreeRoot->getHash();
    hash::mixInto(newHash, getFilePath().u8string());

    if (m_saveHash != newHash) {
        if (m_saveHash == m_saveHashWhenSaved) {
            setChanged(Changes::NodeLabelChanged);
        }
        m_saveHash = newHash;
    }
}
