/**
 * QAction for the save file as action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAsAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SaveFileAsAction::SaveFileAsAction()
    : NodeContentsContextMenuActionBase(tr("Save file as\u2026")) {}

void babelwires::SaveFileAsAction::actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();
    const NodeId elementId = model.getNodeId();

    // Since formats are immuatable and live in the registry, they can be accessed outside a scope.
    const FileTypeEntry* fileFormatInformation = nullptr;
    {
        AccessModelScope scope(projectGraphModel);
        const Project& project = scope.getProject();
        const Node* const node = project.getNode(elementId);
        if (!node) {
            return;
        }
        const FileNode* const fileElement = node->tryAs<FileNode>();
        if (!fileElement) {
            return;
        }
        if (isZero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::save)) {
            return;
        }
        fileFormatInformation = fileElement->getFileFormatInformation(projectGraphModel.getContext());
    }
    assert(fileFormatInformation && "This function should not be called when the format is not registered");

    QString newFilePath = showSaveFileDialog(projectGraphModel.getFlowGraphWidget(), *fileFormatInformation);

    if (!newFilePath.isEmpty()) {
        // This is synchronous, but that's probably appropriate for saving.
        ModifyModelScope scope(projectGraphModel);
        std::unique_ptr<Command<Project>> commandPtr =
            std::make_unique<ChangeFileCommand>("Change file path", elementId, newFilePath.toStdString());
        if (scope.getCommandManager().executeAndStealCommand(commandPtr)) {
            scope.getProject().tryToSaveTarget(elementId);
        }
    }
}
