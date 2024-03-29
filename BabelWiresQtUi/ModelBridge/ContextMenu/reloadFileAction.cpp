/**
 * QAction for the reload file action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/reloadFileAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::ReloadFileAction::ReloadFileAction()
    : FeatureContextMenuAction(tr("Reload file")) {}

void babelwires::ReloadFileAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    ModifyModelScope scope(projectBridge);
    FeatureElement* const featureElement = scope.getProject().getFeatureElement(elementId);
    if (!featureElement) {
        return;
    }
    FileElement* const fileElement = featureElement->as<FileElement>();
    if (!fileElement) {
        return;
    }
    if (isZero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
        return;
    }
    scope.getProject().tryToReloadSource(elementId);
}
