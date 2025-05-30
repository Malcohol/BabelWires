/**
 * The command which removes content from a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <unordered_set>

namespace babelwires {
    class Project;
    struct NodeData;

    /// Remove a Node or a connection or a mixture of both from the project.
    /// All the connections of a Node are always removed when it is.
    /// When the UI issues nofications about a removed connection, it might notify about
    /// connections before or after the elements themselves.
    /// This class is designed to allow that to happen in any order, by having the
    /// first command to subsume all the rest, and then have the combined operation
    /// executed in a consistent way.
    class RemoveNodeCommand : public SimpleCommand<Project> {
      public:
        CLONEABLE(RemoveNodeCommand);
        /// A default constructed object cannot be initiatialized until addNodeToRemove
        /// is called or it subsumes a command with data.
        RemoveNodeCommand(std::string commandName);

        /// Remove the given node.
        RemoveNodeCommand(std::string commandName, NodeId nodeId);

        /// Remove the described connection.
        RemoveNodeCommand(std::string commandName, ConnectionDescription connection);

        RemoveNodeCommand(std::string commandName, std::vector<NodeId> nodes, std::vector<ConnectionDescription> connections);

        RemoveNodeCommand(const RemoveNodeCommand& other);

        virtual ~RemoveNodeCommand();

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        void addNodeToRemove(NodeId nodeId);

      private:
        using ConnectionSet = std::unordered_set<ConnectionDescription>;

        /// Add the connection, checking whether it actually represents several connections in the model,
        /// because of collapsed compound features in the UI.
        /// Returns false if the connection is not found in the model.
        bool addConnection(const babelwires::ConnectionDescription& desc, ConnectionSet& connectionSet,
                           const babelwires::Project& project);

      private:
        std::vector<NodeId> m_nodeIds;

        // Note: This can be expanded during initialization.
        std::vector<ConnectionDescription> m_connections;

        // Post initialization data

        std::vector<std::unique_ptr<NodeData>> m_nodesToRestore;
    };

} // namespace babelwires
