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
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

namespace babelwires {
    class Project;
    struct NodeData;

    /// Add a Node to the project.
    class AddNodeCommand : public SimpleCommand<Project> {
      public:
        CLONEABLE(AddNodeCommand);
        /// Create a command which adds the given node.
        /// NOTE: An expanded path entry is always added for the root path, so the constructor asserts it is not already present.
        AddNodeCommand(std::string commandName, std::unique_ptr<NodeData> nodeToAdd);
        AddNodeCommand(const AddNodeCommand& other);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        /// An add can subsume a MoveNodeCommand.
        // TODO This would not be necessary if the UI waited for the engine to add elements before it added nodes.
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        /// For use after addition.
        NodeId getNodeId() const;

      private:
        std::unique_ptr<NodeData> m_elementToAdd;
    };

} // namespace babelwires
