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

babelwires::ChangeFileCommand::ChangeFileCommand(std::string commandName, NodeId elementId, std::filesystem::path newFilePath)
    : SimpleCommand(commandName)
    , m_nodeId(elementId)
    , m_newFilePath(std::move(newFilePath)) {}

bool babelwires::ChangeFileCommand::initialize(const Project& project) {
    const Node* const element = project.getNode(m_nodeId);

    if (!element) {
        return false;
    }

    const FileNode* const fileElement = element->as<FileNode>();

    if (!fileElement) {
        return false;
    }

    m_oldFilePath = fileElement->getFilePath();
    return true;
}

void babelwires::ChangeFileCommand::execute(Project& project) const {
    Node* const element = project.getNode(m_nodeId);
    assert(element && "The element should already be in the project");
    FileNode* const fileElement = element->as<FileNode>();
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_newFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
        project.tryToReloadSource(m_nodeId);
    }
}

void babelwires::ChangeFileCommand::undo(Project& project) const {
    Node* const element = project.getNode(m_nodeId);
    assert(element && "The element should already be in the project");
    FileNode* const fileElement = element->as<FileNode>();
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_oldFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
        project.tryToReloadSource(m_nodeId);
    }
}
