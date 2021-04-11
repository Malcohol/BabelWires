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
#include "BabelWires/FileFormat/fileFormat.hpp"
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
    const FileFormat* fileFormat = nullptr;
    {
        AccessModelScope scope(projectBridge);
        const Project& project = scope.getProject();
        const FeatureElement* const f = project.getFeatureElement(elementId);
        if (!f) {
            return;
        }
        const FileElement* const fileElement = dynamic_cast<const FileElement*>(f);
        if (!fileElement) {
            return;
        }
        if (isZero(fileElement->getSupportedFileOperations() & FileElement::FileOperations::reload)) {
            return;
        }
        fileFormat =
            projectBridge.getContext().m_fileFormatReg.getEntryByIdentifier(fileElement->getFileFormatIdentifier());
    }
    assert(fileFormat && "This function should not be called when the format is not registered");

    QString newFilePath = showOpenFileDialog(projectBridge.getFlowGraphWidget(), *fileFormat);
    if (!newFilePath.isEmpty()) {
        projectBridge.scheduleCommand(
            std::make_unique<ChangeFileCommand>("Change file path", elementId, newFilePath.toStdString()));
    }
}
