/**
 * SourceFileNodes are Nodes which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>

#include <Common/Log/userLogger.hpp>

babelwires::SourceFileNode::SourceFileNode(const ProjectContext& context, UserLogger& userLogger,
                                                 const SourceFileNodeData& data, NodeId newId)
    : FileNode(data, newId) {
    reload(context, userLogger);
}

const babelwires::SourceFileNodeData& babelwires::SourceFileNode::getNodeData() const {
    return static_cast<const SourceFileNodeData&>(Node::getNodeData());
}

babelwires::SourceFileNodeData& babelwires::SourceFileNode::getNodeData() {
    return static_cast<SourceFileNodeData&>(Node::getNodeData());
}

babelwires::ValueTreeNode* babelwires::SourceFileNode::doGetOutputNonConst() {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::SourceFileNode::getOutput() const {
    return m_valueTreeRoot.get();
}

void babelwires::SourceFileNode::setValueTreeRoot(std::unique_ptr<ValueTreeRoot> root) {
    setValueTrees("File", nullptr, root.get());
    m_valueTreeRoot = std::move(root);
}

void babelwires::SourceFileNode::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        setValueTrees("File", nullptr, m_valueTreeRoot.get());
    }
}

std::filesystem::path babelwires::SourceFileNode::getFilePath() const {
    return getNodeData().m_filePath;
}

void babelwires::SourceFileNode::setFilePath(std::filesystem::path newFilePath) {
    if (newFilePath != getNodeData().m_filePath) {
        getNodeData().m_filePath = std::move(newFilePath);
        setChanged(Changes::FileChanged);
    }
}

const babelwires::FileTypeEntry*
babelwires::SourceFileNode::getFileFormatInformation(const ProjectContext& context) const {
    // TODO: tryGetRegisteredEntry
   try {
        const FileTypeEntry& format = context.m_sourceFileFormatReg.getRegisteredEntry(getNodeData().m_factoryIdentifier);
        return &format;
    } catch (const RegistryException& e) {
    }
    return nullptr;
}

babelwires::FileNode::FileOperations babelwires::SourceFileNode::getSupportedFileOperations() const {
    return FileOperations::reload;
}

bool babelwires::SourceFileNode::reload(const ProjectContext& context, UserLogger& userLogger) {
    const SourceFileNodeData& data = getNodeData();

    try {
        const SourceFileFormat& format = context.m_sourceFileFormatReg.getRegisteredEntry(data.m_factoryIdentifier);
        setFactoryName(format.getName());

        if (data.m_filePath.empty()) {
            throw ModelException() << "No file name";
        }

        setValueTreeRoot(format.loadFromFile(data.m_filePath, context, userLogger));
        clearInternalFailure();
        return true;
    } catch (const RegistryException& e) {
        userLogger.logError() << "Could not create Source File Feature id=" << data.m_id << ": " << e.what();
        setFactoryName(data.m_factoryIdentifier);
        setInternalFailure(e.what());
        // A dummy root
        auto failure = std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisType());
        failure->setToDefault();
        setValueTreeRoot(std::move(failure));
    } catch (const BaseException& e) {
        userLogger.logError() << "Source File Feature id=" << data.m_id << " could not be loaded: " << e.what();
        setInternalFailure(e.what());
        // A dummy file root which allows the user to change the file via the context menu.
        auto failure = std::make_unique<ValueTreeRoot>(context.m_typeSystem, FailureType::getThisType());
        failure->setToDefault();
        setValueTreeRoot(std::move(failure));
    }
    return false;
}
