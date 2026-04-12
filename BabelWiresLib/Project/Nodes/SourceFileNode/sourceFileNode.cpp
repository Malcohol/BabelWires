/**
 * SourceFileNodes are Nodes which correspond to a source file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Log/userLogger.hpp>

babelwires::SourceFileNode::SourceFileNode(const Context& context, UserLogger& userLogger,
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
babelwires::SourceFileNode::getFileFormatInformation(const Context& context) const {
    const auto formatResult = context.get<SourceFileFormatRegistry>().getRegisteredEntry(getNodeData().m_factoryIdentifier);
    if (!formatResult) {
        return nullptr;
    }
    return *formatResult;
}

babelwires::FileNode::FileOperations babelwires::SourceFileNode::getSupportedFileOperations() const {
    return FileOperations::reload;
}

bool babelwires::SourceFileNode::reload(const Context& context, UserLogger& userLogger) {
    const SourceFileNodeData& data = getNodeData();

    const auto onFailure = [this, context](std::string error) {
        setInternalFailure(std::move(error));
        // A dummy file root which allows the user to change the file via the context menu.
        auto failure = std::make_unique<ValueTreeRoot>(context.get<TypeSystem>(),
                                                       context.get<TypeSystem>().getRegisteredType<FailureType>());
        failure->setToDefault();
        setValueTreeRoot(std::move(failure));
    };

    const auto formatResult = context.get<SourceFileFormatRegistry>().getRegisteredEntry(data.m_factoryIdentifier);
    if (!formatResult) {
        userLogger.logError() << "Could not create Source File Node id=" << data.m_id << ": "
                              << formatResult.error().toString();
        setFactoryName(data.m_factoryIdentifier);
        onFailure(formatResult.error().toString());
        return false;
    }
    const SourceFileFormat& format = **formatResult;
    setFactoryName(format.getName());

    if (data.m_filePath.empty()) {
        userLogger.logError() << "Source File Node id=" << data.m_id << " could not be loaded: " << "No file name";
        onFailure("No file name");
        return false;
    }

    auto loadResult = format.loadFromFile(data.m_filePath, context, userLogger);
    if (!loadResult) {
        userLogger.logError() << "Source File Node id=" << data.m_id
                              << " could not be loaded: " << loadResult.error().toString();
        onFailure(loadResult.error().toString());
        return false;
    }
    setValueTreeRoot(std::move(*loadResult));
    clearInternalFailure();
    return true;
}
