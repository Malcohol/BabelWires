/**
 * The command which changes the UiSize of a FeatureElement.
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

    class ResizeElementCommand : public SimpleCommand {
      public:
        ResizeElementCommand(std::string commandName, ElementId elementId, UiSize newSize);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

      private:
        ElementId m_elementId;
        UiSize m_newSize;
        UiSize m_oldSize;
    };

} // namespace babelwires
