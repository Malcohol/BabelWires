/**
 * QAction for the remove entry from array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/projectCommandContextMenuAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

babelwires::ProjectCommandContextMenuAction::ProjectCommandContextMenuAction(std::unique_ptr<Command<Project>> command)
    : NodeContentsContextMenuActionBase(command->getName().c_str())
    , m_command(std::move(command))
 {}

 babelwires::ProjectCommandContextMenuAction::ProjectCommandContextMenuAction(QString actionName, std::unique_ptr<Command<Project>> command)
    : NodeContentsContextMenuActionBase(actionName)
    , m_command(std::move(command))
 {}

 babelwires::ProjectCommandContextMenuAction::~ProjectCommandContextMenuAction() = default;

void babelwires::ProjectCommandContextMenuAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                         const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();
    const NodeId elementId = model.getNodeId();
    projectGraphModel.scheduleCommand(m_command->clone());
}
