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
    class AddNodeForOutputTreeValueCommand : public AddNodeForTreeValueCommandBase {
      public:
        AddNodeForOutputTreeValueCommand(std::string commandName, NodeId originalNodeId, Path pathToValue,
                                         UiPosition positionForNewNode);

        bool initializeAndExecute(Project& project) override;
        void execute(Project& project) const override;
        void undo(Project& project) const override;
    };

} // namespace babelwires
