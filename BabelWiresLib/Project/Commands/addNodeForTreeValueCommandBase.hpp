/**
 * The command which adds Nodes to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

namespace babelwires {
    class Project;
    struct NodeData;

    /// Create a new node using the data in an existing input row in the project,
    class AddNodeForTreeValueCommandBase : public Command<Project> {
      public:
        CLONEABLE_ABSTRACT(AddNodeForTreeValueCommandBase);

        AddNodeForTreeValueCommandBase(std::string commandName, NodeId originalNodeId, Path pathToValue, UiPosition positionForNewNode);

        /// For use after addition.
        NodeId getNodeId() const;

        bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        void subsume(std::unique_ptr<Command> subsequentCommand) override;

      protected:
        NodeId m_originalNodeId;
        Path m_pathToValue;
        UiPosition m_positionForNewNode;

        // Post initialization data

        /// This is only set after the command is executed.
        NodeId m_newNodeId = INVALID_NODE_ID;
    };

} // namespace babelwires
