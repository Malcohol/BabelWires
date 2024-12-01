/**
 * The command which adds a modifier to a feature element.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::AddModifierCommand::AddModifierCommand(std::string commandName, NodeId targetId,
                                                   std::unique_ptr<ModifierData> modifierToAdd)
    : CompoundCommand(commandName)
    , m_targetNodeId(targetId)
    , m_modifierToAdd(std::move(modifierToAdd)) {}

bool babelwires::AddModifierCommand::initializeAndExecute(Project& project) {
    const Node* node = project.getNode(m_targetNodeId);

    if (!node) {
        return false;
    }

    const ValueTreeNode* const input = node->getInput();
    if (!input) {
        return false;
    }

    if (!m_modifierToAdd->m_targetPath.tryFollow(*input)) {
        return false;
    }

    if (m_modifierToAdd->as<ConnectionModifierData>()) {
        std::vector<std::unique_ptr<Command>> subcommands;
        // If this is a connection at a compound type, then any local array modification below need
        // to be removed, to avoid array merges.
        // TODO It would probably be better for these modifiers to fail rather than be removed, but at the
        // moment, I don't think modifiers can currently fail/recover based on the presence of other modifiers.
        for (const auto& modifier : node->getEdits().modifierRange(m_modifierToAdd->m_targetPath)) {
            if (m_modifierToAdd->m_targetPath.isStrictPrefixOf(modifier->getTargetPath()) && modifier->as<ArraySizeModifier>()) {
                subcommands.emplace_back(std::make_unique<RemoveModifierCommand>(
                    "Remove modifier subcommand", m_targetNodeId, modifier->getTargetPath()));
            }
        }
        for (auto it = subcommands.rbegin(); it != subcommands.rend(); ++it) {
            addSubCommand(std::move(*it));
        }
    }

    if (const Modifier* const modifier = node->findModifier(m_modifierToAdd->m_targetPath)) {
        addSubCommand(
            std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", m_targetNodeId, m_modifierToAdd->m_targetPath));
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
