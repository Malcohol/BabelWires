/**
 * QAction for the save file as action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/saveFileAsAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/FeatureElements/fileElement.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SaveFileAsAction::SaveFileAsAction()
    : FeatureContextMenuAction(tr("Save file as\u2026")) {}

void babelwires::SaveFileAsAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    // Since formats are immuatable and live in the registry, they can be accessed outside a scope.
    const FileTypeEntry* fileFormatInformation = nullptr;
    {
        AccessModelScope scope(projectBridge);
        const Project& project = scope.getProject();
        const FeatureElement* const featureElement = project.getFeatureElement(elementId);
        if (!featureElement) {
            return;
        }
        const FileElement* const fileElement = featureElement->as<FileElement>();
        if (!fileElement) {
            return;
        }
        if (isZero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::save)) {
            return;
        }
        fileFormatInformation = fileElement->getFileFormatInformation(projectBridge.getContext());
    }
    assert(fileFormatInformation && "This function should not be called when the format is not registered");

    QString newFilePath = showSaveFileDialog(projectBridge.getFlowGraphWidget(), *fileFormatInformation);

    if (!newFilePath.isEmpty()) {
        // This is synchronous, but that's probably appropriate for saving.
        ModifyModelScope scope(projectBridge);
        std::unique_ptr<Command<Project>> commandPtr =
            std::make_unique<ChangeFileCommand>("Change file path", elementId, newFilePath.toStdString());
        if (scope.getCommandManager().executeAndStealCommand(commandPtr)) {
            scope.getProject().tryToSaveTarget(elementId);
        }
    }
}
