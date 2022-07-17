/**
 * The command which changes the UiSize of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/resizeElementCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::ResizeElementCommand::ResizeElementCommand(std::string commandName, ElementId elementId, UiSize newSize)
    : SimpleCommand(std::move(commandName))
    , m_elementId(elementId)
    , m_newSize(newSize) {}

bool babelwires::ResizeElementCommand::initialize(const Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_elementId);
    if (!element) {
        return false;
    }
    m_oldSize = element->getUiSize();
    return true;
}

void babelwires::ResizeElementCommand::execute(Project& project) const {
    project.setElementContentsSize(m_elementId, m_newSize);
}

void babelwires::ResizeElementCommand::undo(Project& project) const {
    project.setElementContentsSize(m_elementId, m_oldSize);
}

bool babelwires::ResizeElementCommand::shouldSubsume(const Command& subsequentCommand,
                                                     bool thisIsAlreadyExecuted) const {
    const auto* resizeElementCommand = subsequentCommand.as<ResizeElementCommand>();
    if (!resizeElementCommand) {
        return false;
    }
    if (m_elementId != resizeElementCommand->m_elementId) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // If the command has already been initialized, subsume the command only if the
    // commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((resizeElementCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::ResizeElementCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<ResizeElementCommand>() && "subsume should not have been called");
    ResizeElementCommand* resizeElementCommand = static_cast<ResizeElementCommand*>(subsequentCommand.get());

    m_newSize = resizeElementCommand->m_newSize;
    setTimestamp(resizeElementCommand->getTimestamp());
}
