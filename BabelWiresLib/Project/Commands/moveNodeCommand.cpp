/**
 * The command which changes the UiPosition of a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::MoveNodeCommand::MoveNodeCommand(std::string commandName, NodeId elementId, UiPosition newPosition)
    : SimpleCommand(std::move(commandName))
    , m_newPositions{std::pair{elementId, newPosition}} {}

bool babelwires::MoveNodeCommand::initialize(const Project& project) {
    for (const auto& [elementId, _] : m_newPositions) {
        const Node* node = project.getNode(elementId);
        if (!node) {
            return false;
        }
        m_oldPositions.insert(std::pair{elementId, node->getUiPosition()});
    }
    return true;
}

void babelwires::MoveNodeCommand::execute(Project& project) const {
    for (const auto& [elementId, newPosition] : m_newPositions) {
        project.setNodePosition(elementId, newPosition);
    }
}

void babelwires::MoveNodeCommand::undo(Project& project) const {
    for (const auto& [elementId, oldPosition] : m_oldPositions) {
        project.setNodePosition(elementId, oldPosition);
    }
}

bool babelwires::MoveNodeCommand::shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const {
    const auto* moveNodeCommand = subsequentCommand.as<MoveNodeCommand>();
    if (!moveNodeCommand) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // If the command has already been initialized, subsume the command only if the
    // commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((moveNodeCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::MoveNodeCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<MoveNodeCommand>() && "subsume should not have been called");
    MoveNodeCommand* moveNodeCommand = static_cast<MoveNodeCommand*>(subsequentCommand.get());
    for (const auto& p : moveNodeCommand->m_newPositions) {
        m_newPositions[p.first] = p.second;
    }
    for (const auto& p : moveNodeCommand->m_oldPositions) {
        // Only override old positions if they didn't exist already.
        if (m_oldPositions.find(p.first) == m_oldPositions.end()) {
            m_oldPositions[p.first] = p.second;
        }
    }
    setTimestamp(moveNodeCommand->getTimestamp());
}

std::optional<babelwires::UiPosition>
babelwires::MoveNodeCommand::getPositionForOnlyNode(NodeId elementId) const {
    if (m_newPositions.size() == 1) {
        return {m_newPositions.begin()->second};
    } else {
        return {};
    }
}
