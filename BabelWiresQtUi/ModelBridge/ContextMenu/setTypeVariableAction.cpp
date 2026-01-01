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
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <QInputDialog>

babelwires::SetTypeVariableAction::SetTypeVariableAction(ProjectDataLocation pathToGenericType,
                                                         unsigned int variableIndex, bool isSetTypeVariable)
    : NodeContentsContextMenuActionBase(getActionName(variableIndex, isSetTypeVariable).c_str())
    , m_locationOfGenericType(std::move(pathToGenericType))
    , m_variableIndex(variableIndex)
    , m_isSetTypeVariable(isSetTypeVariable) {}

std::string babelwires::SetTypeVariableAction::getActionName(unsigned int variableIndex, bool isSetTypeVariable) {
    std::ostringstream oss;
    oss << (isSetTypeVariable ? "Set" : "Reset") << " type variable " << TypeVariableData{variableIndex}.toString();
    return oss.str();
}

void babelwires::SetTypeVariableAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                        const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();

    TypeExp currentAssignment;

    // Don't keep the project locked.
    {
        AccessModelScope scope(projectGraphModel);
        const Node* const node = scope.getProject().getNode(m_locationOfGenericType.getNodeId());

        const babelwires::ValueTreeNode* const input =
            tryFollowPath(m_locationOfGenericType.getPathToValue(), *node->getInput());
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

    std::ostringstream text;
    text << "GenericType at " << m_locationOfGenericType;

    std::vector<TypeExp> allowedTypes;
    for (const auto& typeId : projectGraphModel.getContext().m_typeSystem.getAllRegisteredTypes()) {
        allowedTypes.emplace_back(typeId);
    }

    const std::string actionName = getActionName(m_variableIndex, m_isSetTypeVariable);

    bool ok = true;
    TypeExp newType;
    if (m_isSetTypeVariable) {
        newType = TypeInputDialog::getType(nullptr, actionName.c_str(), text.str().c_str(), allowedTypes,
                                           currentAssignment, &ok);
    }
    if (ok) {
        projectGraphModel.scheduleCommand(std::make_unique<SetTypeVariableCommand>(
            actionName, m_locationOfGenericType.getNodeId(), m_locationOfGenericType.getPathToValue(), m_variableIndex,
            std::move(newType)));
    }
}
