/**
 * QAction which opens a TypeInputDialog to set a type variable.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/setTypeVariableAction.hpp>

#include <BabelWiresQtUi/Dialogs/typeInputDialog.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>
#include <BabelWiresLib/Project/Commands/setTypeVariableCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <QInputDialog>

babelwires::SetTypeVariableAction::SetTypeVariableAction(babelwires::Path pathToGenericType, unsigned int variableIndex)
    : NodeContentsContextMenuActionBase(tr("Set type variable"))
    , m_pathToGenericType(std::move(pathToGenericType))
    , m_variableIndex(variableIndex) {}

void babelwires::SetTypeVariableAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                        const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();
    const NodeId elementId = model.getNodeId();

    TypeRef currentAssignment;
    
    // Don't keep the project locked.
    {
        AccessModelScope scope(projectGraphModel);
        const Node* const node = scope.getProject().getNode(elementId);

        const babelwires::ValueTreeNode* const input = tryFollowPath(m_pathToGenericType, *node->getInput());
        if (!input) {
            return;
        }
        if (const GenericType* const genericType = input->getType().as<GenericType>()) {
            if (m_variableIndex >= genericType->getNumVariables()) {
                return;
            } else {
                currentAssignment = genericType->getTypeAssignment(input->getValue(), m_variableIndex);
            }
        } else {
            return;
        }
    }

    bool ok;
    std::ostringstream text;
    text << "GenericType at " << m_pathToGenericType;

    std::vector<TypeRef> allowedTypes;
    for (const auto& typeId : projectGraphModel.getContext().m_typeSystem.getAllRegisteredTypes()) {
        allowedTypes.emplace_back(typeId);
    }

    TypeRef newType = TypeInputDialog::getType(nullptr, "Set type variable", text.str().c_str(), allowedTypes,
                                               currentAssignment, &ok);

    if (ok) {
        projectGraphModel.scheduleCommand(std::make_unique<SetTypeVariableCommand>(
            "Set type variable", elementId, m_pathToGenericType, m_variableIndex, std::move(newType)));
    }
}
