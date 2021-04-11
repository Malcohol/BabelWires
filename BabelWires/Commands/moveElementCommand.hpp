/**
 * The command which changes the UiPosition of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"
#include "BabelWires/Project/uiPosition.hpp"

#include <map>
#include <optional>

namespace babelwires {

    /// The command which changes the UiPosition of a FeatureElement.
    class MoveElementCommand : public SimpleCommand {
      public:
        MoveElementCommand(std::string commandName, ElementId elementId, UiPosition newPosition);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        /// If this command only carries a new position for the given element, return its position.
        /// This method exists allow the AddElementCommand to subsume this one.
        std::optional<UiPosition> getPositionForOnlyNode(ElementId elementId) const;

      private:
        std::map<ElementId, UiPosition> m_newPositions;
        std::map<ElementId, UiPosition> m_oldPositions;
    };

} // namespace babelwires
