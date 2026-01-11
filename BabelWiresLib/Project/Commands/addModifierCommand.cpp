/**
 * The command which adds a modifier to a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

babelwires::AddModifierCommand::AddModifierCommand(std::string commandName, NodeId targetId,
                                                   std::unique_ptr<ModifierData> modifierToAdd)
    : CompoundCommand(commandName)
    , m_targetNodeId(targetId)
    , m_modifierToAdd(std::move(modifierToAdd)) {
    // TODO This modifier might be reused for generic operations, so it may not be possible to assert the following.
    assert(!m_modifierToAdd->tryAs<ConnectionModifierData>() &&
           "Connections should be added with the AddConnectionCommand");
}

babelwires::AddModifierCommand::AddModifierCommand(const AddModifierCommand& other)
    : CompoundCommand(other)
    , m_targetNodeId(other.m_targetNodeId)
    , m_modifierToAdd(other.m_modifierToAdd->clone()) {}

bool babelwires::AddModifierCommand::initializeAndExecute(Project& project) {
    const Node* node = project.getNode(m_targetNodeId);

    if (!node) {
        return false;
    }

    const ValueTreeNode* const input = node->getInput();
    if (!input) {
        return false;
    }

    if (!tryFollowPath(m_modifierToAdd->m_targetPath, *input)) {
        return false;
    }

    if (const Modifier* const modifier = node->findModifier(m_modifierToAdd->m_targetPath)) {
        addSubCommand(std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", m_targetNodeId,
                                                              m_modifierToAdd->m_targetPath));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.addModifier(m_targetNodeId, *m_modifierToAdd);

    return true;
}

void babelwires::AddModifierCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.addModifier(m_targetNodeId, *m_modifierToAdd);
}

void babelwires::AddModifierCommand::undo(Project& project) const {
    project.removeModifier(m_targetNodeId, m_modifierToAdd->m_targetPath);
    CompoundCommand::undo(project);
}
