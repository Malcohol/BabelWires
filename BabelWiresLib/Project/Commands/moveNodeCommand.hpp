/**
 * The command which changes the UiPosition of a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <map>
#include <optional>

namespace babelwires {
    class Project;

    /// The command which changes the UiPosition of a Node.
    class MoveNodeCommand : public SimpleCommand<Project> {
      public:
        MoveNodeCommand(std::string commandName, ElementId elementId, UiPosition newPosition);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        /// If this command only carries a new position for the given element, return its position.
        /// This method exists allow the AddNodeCommand to subsume this one.
        std::optional<UiPosition> getPositionForOnlyNode(ElementId elementId) const;

      private:
        std::map<ElementId, UiPosition> m_newPositions;
        std::map<ElementId, UiPosition> m_oldPositions;
    };

} // namespace babelwires
