/**
 * The command which changes the UiSize of a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/resizeNodeCommand.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::ResizeNodeCommand::ResizeNodeCommand(std::string commandName, NodeId elementId, UiSize newSize)
    : SimpleCommand(std::move(commandName))
    , m_elementId(elementId)
    , m_newSize(newSize) {}

bool babelwires::ResizeNodeCommand::initialize(const Project& project) {
    const Node* element = project.getNode(m_elementId);
    if (!element) {
        return false;
    }
    m_oldSize = element->getUiSize();
    return true;
}

void babelwires::ResizeNodeCommand::execute(Project& project) const {
    project.setNodeContentsSize(m_elementId, m_newSize);
}

void babelwires::ResizeNodeCommand::undo(Project& project) const {
    project.setNodeContentsSize(m_elementId, m_oldSize);
}

bool babelwires::ResizeNodeCommand::shouldSubsume(const Command& subsequentCommand,
                                                     bool thisIsAlreadyExecuted) const {
    const auto* resizeNodeCommand = subsequentCommand.as<ResizeNodeCommand>();
    if (!resizeNodeCommand) {
        return false;
    }
    if (m_elementId != resizeNodeCommand->m_elementId) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // If the command has already been initialized, subsume the command only if the
    // commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((resizeNodeCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::ResizeNodeCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<ResizeNodeCommand>() && "subsume should not have been called");
    ResizeNodeCommand* resizeNodeCommand = static_cast<ResizeNodeCommand*>(subsequentCommand.get());

    m_newSize = resizeNodeCommand->m_newSize;
    setTimestamp(resizeNodeCommand->getTimestamp());
}
