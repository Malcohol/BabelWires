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
        ProjectCommandContextMenuAction(std::unique_ptr<Command<Project>> command);
        ~ProjectCommandContextMenuAction();

        virtual void actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const override;

      private:
        std::unique_ptr<Command<Project>> m_command;
    };

} // namespace babelwires
