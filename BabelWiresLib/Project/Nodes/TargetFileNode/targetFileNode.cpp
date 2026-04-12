/**
 * TargetFileNodes are Nodes which correspond to a target file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Log/userLogger.hpp>

#include <fstream>

babelwires::TargetFileNode::TargetFileNode(const Context& context, UserLogger& userLogger,
                                                 const TargetFileNodeData& data, NodeId newId)
    : FileNode(data, newId) {
    const NodeData& nodeData = getNodeData();
    setFactoryName(nodeData.m_factoryIdentifier);
    const auto factoryResult = context.getService<TargetFileFormatRegistry>().getRegisteredEntry(nodeData.m_factoryIdentifier);
    if (!factoryResult) {
        setInternalFailure(factoryResult.error().toString());
        setValueTreeRoot(std::make_unique<ValueTreeRoot>(context.getService<TypeSystem>(), context.getService<TypeSystem>().getRegisteredType<FailureType>()));
        userLogger.logError() << "Failed to create target id=" << nodeData.m_id
                                << ": " << factoryResult.error().toString();
        return;
    }
    const TargetFileFormat& factory = **factoryResult;
    setFactoryName(factory.getName());
    auto newFeature = factory.createNewValue(context);
    newFeature->setToDefault();
    setValueTreeRoot(std::move(newFeature));
}

babelwires::TargetFileNode::~TargetFileNode() = default;

const babelwires::TargetFileNodeData& babelwires::TargetFileNode::getNodeData() const {
    return static_cast<const TargetFileNodeData&>(Node::getNodeData());
}

babelwires::TargetFileNodeData& babelwires::TargetFileNode::getNodeData() {
    return static_cast<TargetFileNodeData&>(Node::getNodeData());
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
    return getNodeData().m_filePath;
}

void babelwires::TargetFileNode::setFilePath(std::filesystem::path newFilePath) {
    if (newFilePath != getNodeData().m_filePath) {
        getNodeData().m_filePath = std::move(newFilePath);
        setChanged(Changes::FileChanged);
        updateSaveHash();
    }
}

const babelwires::FileTypeEntry*
babelwires::TargetFileNode::getFileFormatInformation(const Context& context) const {
    const auto formatResult = context.getService<TargetFileFormatRegistry>().getRegisteredEntry(getNodeData().m_factoryIdentifier);
    if (!formatResult) {
        return nullptr;
    }
    return *formatResult;
}

babelwires::FileNode::FileOperations babelwires::TargetFileNode::getSupportedFileOperations() const {
    return FileOperations::save;
}

bool babelwires::TargetFileNode::save(const Context& context, UserLogger& userLogger) {
    const auto& data = getNodeData();
    if (isFailed()) {
        userLogger.logError() << "Cannot write output for failed TargetFileNode (id=" << data.m_id << ")";
        return false;
    }
    if (data.m_filePath.empty()) {
        userLogger.logError() << "Cannot write output for TargetFileNode when there is no file path (id="
                              << data.m_id << ")";
        return false;
    }
    const TargetFileFormat* format = context.getService<TargetFileFormatRegistry>().getEntryByIdentifier(data.m_factoryIdentifier);
    assert(format && "FileFeature with unregistered file format");
    const auto writeResult = format->writeToFile(context, userLogger, *m_valueTreeRoot, data.m_filePath);
    if (!writeResult) {
        userLogger.logError() << "Failed to write output for TargetFileNode (id=" << data.m_id
                              << "): " << writeResult.error().toString();
        return false;
    }

    if (m_saveHashWhenSaved != m_saveHash) {
        setChanged(Changes::NodeLabelChanged);
        m_saveHashWhenSaved = m_saveHash;
    }
    return true;
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
