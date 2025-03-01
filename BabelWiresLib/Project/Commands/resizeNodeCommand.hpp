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
        CLONEABLE(ResizeNodeCommand);
        ResizeNodeCommand(std::string commandName, NodeId nodeId, UiSize newSize);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

      private:
        NodeId m_nodeId;
        UiSize m_newSize;
        
        // Post initialization data
        
        UiSize m_oldSize;
    };

} // namespace babelwires
