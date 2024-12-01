/**
 * QAction for the save file action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SaveFileAction::SaveFileAction()
    : FeatureContextMenuAction(tr("Save file")) {}

void babelwires::SaveFileAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    ModifyModelScope scope(projectBridge);
    Node* const featureElement = scope.getProject().getFeatureElement(elementId);
    if (!featureElement) {
        return;
    }
    FileNode* const fileElement = featureElement->as<FileNode>();
    if (!fileElement) {
        return;
    }
    if (isZero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::save)) {
        return;
    }
    scope.getProject().tryToSaveTarget(elementId);
}
