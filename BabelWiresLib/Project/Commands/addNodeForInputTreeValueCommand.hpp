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
#include <BabelWiresLib/Project/Commands/addNodeForTreeValueCommandBase.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

namespace babelwires {
    class Project;
    struct NodeData;

    /// Create a new node using the data in an existing input row in the project,
    class AddNodeForInputTreeValueCommand : public AddNodeForTreeValueCommandBase {
      public:
        CLONEABLE(AddNodeForInputTreeValueCommand);

        enum class RelationshipToOldNode {
            /// The old value will now be assigned its value from the new node. All modifiers move to the new node.
            Source,
            /// The old tree is untouched. The new tree is a copy.
            Copy
        };

        AddNodeForInputTreeValueCommand(std::string commandName, NodeId originalNodeId, Path pathToValue,
                                        UiPosition positionForNewNode, RelationshipToOldNode relationship);

        bool initializeAndExecute(Project& project) override;
        void execute(Project& project) const override;
        void undo(Project& project) const override;

      private:
        RelationshipToOldNode m_relationship;
    };

} // namespace babelwires
