/**
 * The command which changes the UiSize of a Node.
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

    class ResizeNodeCommand : public SimpleCommand<Project> {
      public:
        ResizeNodeCommand(std::string commandName, ElementId elementId, UiSize newSize);

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
