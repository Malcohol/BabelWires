/**
 * Action which applies a command to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ModelBridge/ContextMenu/nodeContentsContextMenuActionBase.hpp>

#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class Project;

    /// Action which applies a command to the project.
    class ProjectCommandContextMenuAction : public NodeContentsContextMenuActionBase {
      public:
        /// The action name infers its name from the command.
        ProjectCommandContextMenuAction(std::unique_ptr<Command<Project>> command);
        /// This constructor allows the action name to differ from the command name.
        /// The former is presented in the UI and the latter in the Undo stack.
        ProjectCommandContextMenuAction(QString actionName, std::unique_ptr<Command<Project>> command);
        ~ProjectCommandContextMenuAction();

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        std::unique_ptr<Command<Project>> m_command;
    };

} // namespace babelwires
