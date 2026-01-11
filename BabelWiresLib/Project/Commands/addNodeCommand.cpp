/**
 * The command which adds Nodes to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::AddNodeCommand::AddNodeCommand(std::string commandName, std::unique_ptr<NodeData> nodeToAdd)
    : SimpleCommand(std::move(commandName))
    , m_elementToAdd(std::move(nodeToAdd)) {
    assert(m_elementToAdd);
    assert((std::find(m_elementToAdd->m_expandedPaths.begin(), m_elementToAdd->m_expandedPaths.end(), Path()) ==
            m_elementToAdd->m_expandedPaths.end()) &&
           "The root is always expanded by default.");
    m_elementToAdd->m_expandedPaths.emplace_back(Path());
}

babelwires::AddNodeCommand::AddNodeCommand(const AddNodeCommand& other)
    : SimpleCommand(other)
    , m_elementToAdd(other.m_elementToAdd->clone()) {}

void babelwires::AddNodeCommand::execute(Project& project) const {
    NodeId newId = project.addNode(*m_elementToAdd);
    m_elementToAdd->m_id = newId;
}

bool babelwires::AddNodeCommand::initialize(const Project& project) {
    return true;
}

babelwires::NodeId babelwires::AddNodeCommand::getNodeId() const {
    return m_elementToAdd->m_id;
}

void babelwires::AddNodeCommand::undo(Project& project) const {
    project.removeNode(getNodeId());
}

bool babelwires::AddNodeCommand::shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const {
    const auto* moveNodeCommand = subsequentCommand.tryAs<MoveNodeCommand>();
    if (!moveNodeCommand) {
        return false;
    }
    if (!moveNodeCommand->getPositionForOnlyNode(m_elementToAdd->m_id)) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // Subsume the command only if the commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((moveNodeCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::AddNodeCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->tryAs<MoveNodeCommand>() && "subsume should not have been called");
    MoveNodeCommand* moveNodeCommand = static_cast<MoveNodeCommand*>(subsequentCommand.get());
    if (auto optionalPosition = moveNodeCommand->getPositionForOnlyNode(m_elementToAdd->m_id)) {
        m_elementToAdd->m_uiData.m_uiPosition = *optionalPosition;
    }
}
