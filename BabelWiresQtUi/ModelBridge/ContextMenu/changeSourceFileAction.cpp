/**
 * QAction for the Change Source File action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/changeSourceFileAction.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Registry/fileTypeRegistry.hpp>

babelwires::ChangeSourceFileAction::ChangeSourceFileAction()
    : NodeContentsContextMenuActionBase(tr("Open file\u2026")) {}

void babelwires::ChangeSourceFileAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                         const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();
    const NodeId elementId = model.getNodeId();

    // Since formats are immuatable and live in the registry, they can be accessed outside a scope.
    const FileTypeEntry* fileFormatInformation = nullptr;
    {
        AccessModelScope scope(projectGraphModel);
        const Project& project = scope.getProject();
        const Node* const f = project.getNode(elementId);
        if (!f) {
            return;
        }
        const FileNode* const fileElement = f->as<FileNode>();
        if (!fileElement) {
            return;
        }
        if (isZero(fileElement->getSupportedFileOperations() & FileNode::FileOperations::reload)) {
            return;
        }
        fileFormatInformation = fileElement->getFileFormatInformation(projectGraphModel.getContext());
    }
    assert(fileFormatInformation && "This function should not be called when the format is not registered");

    QString newFilePath = showOpenFileDialog(projectGraphModel.getFlowGraphWidget(), *fileFormatInformation);
    if (!newFilePath.isEmpty()) {
        projectGraphModel.scheduleCommand(
            std::make_unique<ChangeFileCommand>("Change file path", elementId, newFilePath.toStdString()));
    }
}
