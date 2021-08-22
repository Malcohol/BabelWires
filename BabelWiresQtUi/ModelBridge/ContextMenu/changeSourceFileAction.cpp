/**
 * QAction for the Change Source File action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/changeSourceFileAction.hpp"

#include "BabelWiresQtUi/ModelBridge/accessModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/Utilities/fileDialogs.hpp"

#include "BabelWires/Commands/changeFileCommand.hpp"
#include "BabelWires/FileFormat/sourceFileFormat.hpp"
#include "Common/Registry/fileTypeRegistry.hpp"
#include "BabelWires/Project/FeatureElements/fileElement.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWiresQtUi/uiProjectContext.hpp"

babelwires::ChangeSourceFileAction::ChangeSourceFileAction()
    : FeatureContextMenuAction(tr("Open file\u2026")) {}

void babelwires::ChangeSourceFileAction::actionTriggered(babelwires::FeatureModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    // Since formats are immuatable and live in the registry, they can be accessed outside a scope.
    const FileTypeEntry* fileFormatInformation = nullptr;
    {
        AccessModelScope scope(projectBridge);
        const Project& project = scope.getProject();
        const FeatureElement* const f = project.getFeatureElement(elementId);
        if (!f) {
            return;
        }
        const FileElement* const fileElement = f->as<FileElement>();
        if (!fileElement) {
            return;
        }
        if (isZero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
            return;
        }
        fileFormatInformation = fileElement->getFileFormatInformation(projectBridge.getContext());
    }
    assert(fileFormatInformation && "This function should not be called when the format is not registered");

    QString newFilePath = showOpenFileDialog(projectBridge.getFlowGraphWidget(), *fileFormatInformation);
    if (!newFilePath.isEmpty()) {
        projectBridge.scheduleCommand(
            std::make_unique<ChangeFileCommand>("Change file path", elementId, newFilePath.toStdString()));
    }
}
