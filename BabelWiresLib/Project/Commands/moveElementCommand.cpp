/**
 * The command which changes the UiPosition of a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/moveElementCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/node.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::MoveElementCommand::MoveElementCommand(std::string commandName, ElementId elementId, UiPosition newPosition)
    : SimpleCommand(std::move(commandName))
    , m_newPositions{std::pair{elementId, newPosition}} {}

bool babelwires::MoveElementCommand::initialize(const Project& project) {
    for (const auto& [elementId, _] : m_newPositions) {
        const Node* element = project.getFeatureElement(elementId);
        if (!element) {
            return false;
        }
        m_oldPositions.insert(std::pair{elementId, element->getUiPosition()});
    }
    return true;
}

void babelwires::MoveElementCommand::execute(Project& project) const {
    for (const auto& [elementId, newPosition] : m_newPositions) {
        project.setElementPosition(elementId, newPosition);
    }
}

void babelwires::MoveElementCommand::undo(Project& project) const {
    for (const auto& [elementId, oldPosition] : m_oldPositions) {
        project.setElementPosition(elementId, oldPosition);
    }
}

bool babelwires::MoveElementCommand::shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const {
    const auto* moveElementCommand = subsequentCommand.as<MoveElementCommand>();
    if (!moveElementCommand) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // If the command has already been initialized, subsume the command only if the
    // commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((moveElementCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::MoveElementCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<MoveElementCommand>() && "subsume should not have been called");
    MoveElementCommand* moveElementCommand = static_cast<MoveElementCommand*>(subsequentCommand.get());
    for (const auto& p : moveElementCommand->m_newPositions) {
        m_newPositions[p.first] = p.second;
    }
    for (const auto& p : moveElementCommand->m_oldPositions) {
        // Only override old positions if they didn't exist already.
        if (m_oldPositions.find(p.first) == m_oldPositions.end()) {
            m_oldPositions[p.first] = p.second;
        }
    }
    setTimestamp(moveElementCommand->getTimestamp());
}

std::optional<babelwires::UiPosition>
babelwires::MoveElementCommand::getPositionForOnlyNode(ElementId elementId) const {
    if (m_newPositions.size() == 1) {
        return {m_newPositions.begin()->second};
    } else {
        return {};
    }
}
