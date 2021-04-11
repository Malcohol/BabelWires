/**
 * The command which changes the file of a FileElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Commands/changeFileCommand.hpp"

#include "BabelWires/Project/FeatureElements/fileElement.hpp"
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::ChangeFileCommand::ChangeFileCommand(std::string commandName, ElementId elementId, std::string newFilePath)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_newFilePath(std::move(newFilePath)) {}

bool babelwires::ChangeFileCommand::initialize(const Project& project) {
    const FeatureElement* const element = project.getFeatureElement(m_elementId);

    if (!element) {
        return false;
    }

    const FileElement* const fileElement = dynamic_cast<const FileElement*>(element);

    if (!fileElement) {
        return false;
    }

    m_oldFilePath = fileElement->getFilePath();
    return true;
}

void babelwires::ChangeFileCommand::execute(Project& project) const {
    FeatureElement* const element = project.getFeatureElement(m_elementId);
    assert(element && "The element should already be in the project");
    FileElement* const fileElement = dynamic_cast<FileElement*>(element);
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_newFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        project.tryToReloadSource(m_elementId);
    }
}

void babelwires::ChangeFileCommand::undo(Project& project) const {
    FeatureElement* const element = project.getFeatureElement(m_elementId);
    assert(element && "The element should already be in the project");
    FileElement* const fileElement = dynamic_cast<FileElement*>(element);
    assert(fileElement && "The element should be a file element");
    fileElement->setFilePath(m_oldFilePath);
    if (isNonzero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        project.tryToReloadSource(m_elementId);
    }
}
