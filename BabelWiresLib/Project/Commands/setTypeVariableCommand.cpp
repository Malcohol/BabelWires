/**
 * The command which sets the size of an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/setTypeVariableCommand.hpp>

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericValue.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <cassert>

babelwires::SetTypeVariableCommand::SetTypeVariableCommand(std::string commandName, NodeId nodeId,
                                                           Path pathToGenericType, unsigned int variableIndex,
                                                           TypeExp newType)
    : CompoundCommand(commandName)
    , m_nodeId(nodeId)
    , m_pathToGenericType(std::move(pathToGenericType))
    , m_variableIndex(variableIndex)
    , m_newType(std::move(newType)) {}

bool babelwires::SetTypeVariableCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);

    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    const ValueTreeNode* const genericTypeNode = tryFollowPath(m_pathToGenericType, *input);
    if (!genericTypeNode) {
        return false;
    }

    const GenericType* const genericType = genericTypeNode->getType()->as<GenericType>();
    if (!genericType) {
        return false;
    }
    if (m_variableIndex >= genericType->getNumVariables()) {
        return false;
    }

    if (genericType->getTypeAssignment(genericTypeNode->getValue(), m_variableIndex) == m_newType) {
        // No change needed.
        return false;
    }

    auto newData = std::make_unique<SetTypeVariableModifierData>();
    newData->m_targetPath = m_pathToGenericType;
    if (const Modifier* const currentModifier = nodeToModify->findModifier(m_pathToGenericType)) {
        if (const auto* const setTypeVariableModifier =
                currentModifier->getModifierData().as<SetTypeVariableModifierData>()) {
            // If the modifier already exists, then we can just update it.
            newData->m_typeAssignments = setTypeVariableModifier->m_typeAssignments;
        }
        // TODO This is inefficient and will likely lead to many ValueTreeNodes being unnecessarily marked as changed.
        // I tried harder to avoid this with arrays, but I'm not happy with the approach I took (logic in the project).
        // Approaching this correctly will need a bigger refactor, and is beyond the scope of the current work.
        addSubCommand(std::make_unique<RemoveModifierCommand>("Remove existing modifier subcommand", m_nodeId,
                                                              m_pathToGenericType));
    }
    newData->m_typeAssignments.resize(genericType->getNumVariables());
    newData->m_typeAssignments[m_variableIndex] = m_newType;
    addSubCommand(std::make_unique<AddModifierCommand>("Set type variable", m_nodeId, std::move(newData)));

    return CompoundCommand::initializeAndExecute(project);
}
