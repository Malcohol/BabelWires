/**
 * QAction for the save file action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SaveFileAction::SaveFileAction()
    : NodeContentsContextMenuActionBase(tr("Save file")) {}

void babelwires::SaveFileAction::actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();

    ModifyModelScope scope(projectBridge);
    Node* const node = scope.getProject().getNode(elementId);
    if (!node) {
        return;
    }
    FileNode* const fileElement = node->as<FileNode>();
    if (!fileElement) {
        return;
    }
    if (isZero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::save)) {
        return;
    }
    scope.getProject().tryToSaveTarget(elementId);
}
