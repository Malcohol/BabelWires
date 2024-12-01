/**
 * The command which changes the file of a FileElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::ChangeFileCommand::ChangeFileCommand(std::string commandName, ElementId elementId, std::filesystem::path newFilePath)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_newFilePath(std::move(newFilePath)) {}

bool babelwires::ChangeFileCommand::initialize(const Project& project) {
    const Node* const element = project.getFeatureElement(m_elementId);

    if (!element) {
        return false;
    }

    const FileElement* const fileElement = element->as<FileElement>();

    if (!fileElement) {
        return false;
    }

    m_oldFilePath = fileElement->getFilePath();
    return true;
}

void babelwires::ChangeFileCommand::execute(Project& project) const {
    Node* const element = project.getFeatureElement(m_elementId);
    assert(element && "The element should already be in the project");
    FileElement* const fileElement = element->as<FileElement>();
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_newFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        project.tryToReloadSource(m_elementId);
    }
}

void babelwires::ChangeFileCommand::undo(Project& project) const {
    Node* const element = project.getFeatureElement(m_elementId);
    assert(element && "The element should already be in the project");
    FileElement* const fileElement = element->as<FileElement>();
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_oldFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        project.tryToReloadSource(m_elementId);
    }
}
