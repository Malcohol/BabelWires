/**
 * QAction for the save file as action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/setArraySizeAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>
#include <BabelWiresLib/Project/Commands/setArraySizeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/FileNode/fileNode.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <QInputDialog>

babelwires::SetArraySizeAction::SetArraySizeAction(babelwires::Path pathToArray)
    : NodeContentsContextMenuActionBase(tr("Set array size"))
    , m_pathToArray(std::move(pathToArray)) {}

void babelwires::SetArraySizeAction::actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();

    unsigned int currentSize;
    Range<unsigned int> range;
    // Don't keep the project locked.
    {
        AccessModelScope scope(projectBridge);
        const Node* const node = scope.getProject().getNode(elementId);

        const babelwires::ValueTreeNode* const input = m_pathToArray.tryFollow(*node->getInput());
        auto [compoundFeature, s, r, initialSize] = ValueTreeHelper::getInfoFromArray(input);
        if (!compoundFeature) {
            return;
        }
        currentSize = s;
        range = r;
    }

    bool ok;
    std::ostringstream text;
    text << "Array at " << m_pathToArray;
    int newSize = QInputDialog::getInt(projectBridge.getFlowGraphWidget(), "Set array size", text.str().c_str(),
                                       currentSize, range.m_min, range.m_max, 1, &ok);

    if (ok) {
        projectBridge.scheduleCommand(
            std::make_unique<SetArraySizeCommand>("Set array size", elementId, m_pathToArray, newSize));
    }
}
