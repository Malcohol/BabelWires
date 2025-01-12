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
    class AddNodeForInputTreeValueCommand : public Command<Project> {
      public:
        enum class RelationshipToOldNode
        {
            /// The old value will now be assigned its value from the new node. All modifiers move to the new node.
            Source,
            /// The old tree is untouched. The new tree is a copy.
            Copy
        };

        AddNodeForInputTreeValueCommand(std::string commandName, NodeId originalNodeId, Path pathToValue, UiPosition positionForNewNode, RelationshipToOldNode relationship);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        /// For use after addition.
        NodeId getNodeId() const;

      private:
        NodeId m_originalNodeId;
        Path m_pathToValue;
        RelationshipToOldNode m_relationship;
        UiPosition m_positionForNewNode;

        /// This is only set after the command is executed.
        NodeId m_newNodeId = INVALID_NODE_ID;
    };

} // namespace babelwires
