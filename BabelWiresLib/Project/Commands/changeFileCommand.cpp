/**
 * The command which changes the file of a FileNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::ChangeFileCommand::ChangeFileCommand(std::string commandName, NodeId nodeId, std::filesystem::path newFilePath)
    : SimpleCommand(commandName)
    , m_nodeId(nodeId)
    , m_newFilePath(std::move(newFilePath)) {}

bool babelwires::ChangeFileCommand::initialize(const Project& project) {
    const Node* const node = project.getNode(m_nodeId);

    if (!node) {
        return false;
    }

    const FileNode* const fileElement = node->as<FileNode>();

    if (!fileElement) {
        return false;
    }

    m_oldFilePath = fileElement->getFilePath();
    return true;
}

void babelwires::ChangeFileCommand::execute(Project& project) const {
    Node* const node = project.getNode(m_nodeId);
    assert(node && "The node should already be in the project");
    FileNode* const fileElement = node->as<FileNode>();
    assert(fileElement && "The node should be a file node");
    fileElement->setFilePath(m_newFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
        project.tryToReloadSource(m_nodeId);
    }
}

void babelwires::ChangeFileCommand::undo(Project& project) const {
    Node* const node = project.getNode(m_nodeId);
    assert(node && "The node should already be in the project");
    FileNode* const fileElement = node->as<FileNode>();
    assert(fileElement && "The node should be a file node");
    fileElement->setFilePath(m_oldFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
        project.tryToReloadSource(m_nodeId);
    }
}
