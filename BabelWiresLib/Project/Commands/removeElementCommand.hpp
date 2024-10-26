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
#include <BabelWiresLib/Features/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <unordered_set>

namespace babelwires {
    class Project;
    struct ElementData;

    /// Remove a feature element or a connection or a mixture of both from the project.
    /// All the connections of a feature element are always removed when it is.
    /// When the UI issues nofications about a removed connection, it might notify about
    /// connections before or after the elements themselves.
    /// This class is designed to allow that to happen in any order, by having the
    /// first command to subsume all the rest, and then have the combined operation
    /// executed in a consistent way.
    class RemoveElementCommand : public SimpleCommand<Project> {
      public:
        /// A default constructed object cannot be initiatialized until addElementToRemove
        /// is called or it subsumes a command with data.
        RemoveElementCommand(std::string commandName);

        /// Remove the given element.
        RemoveElementCommand(std::string commandName, ElementId elementId);

        /// Remove the described connection.
        RemoveElementCommand(std::string commandName, ConnectionDescription connection);

        virtual ~RemoveElementCommand();

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        void addElementToRemove(ElementId elementId);

      private:
        using ConnectionSet = std::unordered_set<ConnectionDescription>;

        /// Add the connection, checking whether it actually represents several connections in the model,
        /// because of collapsed compound features in the UI.
        /// Returns false if the connection is not found in the model.
        bool addConnection(const babelwires::ConnectionDescription& desc, ConnectionSet& connectionSet,
                           const babelwires::Project& project);

      private:
        std::vector<ElementId> m_elementIds;

        std::vector<std::unique_ptr<ElementData>> m_elementsToRestore;

        std::vector<ConnectionDescription> m_connections;
    };

} // namespace babelwires
