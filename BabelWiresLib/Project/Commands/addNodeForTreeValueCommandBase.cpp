/**
 * The command which adds Nodes to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addNodeForTreeValueCommandBase.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

babelwires::AddNodeForTreeValueCommandBase::AddNodeForTreeValueCommandBase(std::string commandName,
                                                                             NodeId originalNodeId, Path pathToValue,
                                                                             UiPosition positionForNewNode)
    : Command(commandName)
    , m_originalNodeId(originalNodeId)
    , m_pathToValue(pathToValue)
    , m_positionForNewNode(positionForNewNode) {}

babelwires::NodeId babelwires::AddNodeForTreeValueCommandBase::getNodeId() const {
    return m_newNodeId;
}

bool babelwires::AddNodeForTreeValueCommandBase::shouldSubsume(const Command& subsequentCommand,
                                                                bool thisIsAlreadyExecuted) const {
    const auto* moveNodeCommand = subsequentCommand.as<MoveNodeCommand>();
    if (!moveNodeCommand) {
        return false;
    }
    if (!moveNodeCommand->getPositionForOnlyNode(m_newNodeId)) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // Subsume the command only if the commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((moveNodeCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::AddNodeForTreeValueCommandBase::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<MoveNodeCommand>() && "subsume should not have been called");
    MoveNodeCommand* moveNodeCommand = static_cast<MoveNodeCommand*>(subsequentCommand.get());
    if (auto optionalPosition = moveNodeCommand->getPositionForOnlyNode(m_newNodeId)) {
        m_positionForNewNode = *optionalPosition;
    }
    setTimestamp(moveNodeCommand->getTimestamp());
}
