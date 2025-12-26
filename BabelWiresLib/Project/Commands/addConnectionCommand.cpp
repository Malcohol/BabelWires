/**
 * The command which adds a connection between Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addConnectionCommand.hpp>

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/Commands/setTypeVariableCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableData.hpp>
#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

babelwires::AddConnectionCommand::~AddConnectionCommand() = default;

babelwires::AddConnectionCommand::AddConnectionCommand(std::string commandName, NodeId targetId,
                                                       std::unique_ptr<ConnectionModifierData> modifierToAdd)
    : CompoundCommand(commandName)
    , m_targetNodeId(targetId)
    , m_modifierToAdd(std::move(modifierToAdd)) {}

babelwires::AddConnectionCommand::AddConnectionCommand(const AddConnectionCommand& other)
    : CompoundCommand(other)
    , m_targetNodeId(other.m_targetNodeId)
    , m_modifierToAdd(other.m_modifierToAdd->clone()) {}

bool babelwires::AddConnectionCommand::initializeAndExecute(Project& project) {
    const Node* const targetNode = project.getNode(m_targetNodeId);
    if (!targetNode) {
        return false;
    }

    const ValueTreeNode* const input = targetNode->getInput();
    if (!input) {
        return false;
    }

    const ValueTreeNode* const inputTreeNode = tryFollowPath(m_modifierToAdd->m_targetPath, *input);
    if (!inputTreeNode) {
        return false;
    }

    const Node* const sourceNode = project.getNode(m_modifierToAdd->m_sourceId);
    if (!sourceNode) {
        return false;
    }

    const ValueTreeNode* const output = sourceNode->getOutput();
    if (!output) {
        return false;
    }

    const ValueTreeNode* const outputTreeNode = tryFollowPath(m_modifierToAdd->m_sourcePath, *output);
    if (!outputTreeNode) {
        return false;
    }

    if (containsUnassignedTypeVariable(*inputTreeNode)) {
        std::optional<std::map<std::tuple<Path, unsigned int>, TypeRef>> typeAssignment =
            getTypeVariableAssignments(*outputTreeNode, *inputTreeNode);
        if (typeAssignment) {
            for (const auto& [key, assignedType] : *typeAssignment) {
                const Path& pathToGenericType = std::get<0>(key);
                const unsigned int typeVariableIndex = std::get<1>(key);
                addSubCommand(std::make_unique<SetTypeVariableCommand>(
                    "Set type variable", m_targetNodeId, pathToGenericType, typeVariableIndex, assignedType));
            }
        } else {
            // Could not determine consistent type variable assignments. We could return false here, but that's less
            // informative for the user. So instead, the connection will be added, but the assignment modifier will
            // fail, because the variables are not assigned. 
            // TODO: This is still not that informative.
        }
    }

    {
        std::vector<std::unique_ptr<Command>> subcommands;
        // If this is a connection at a compound type, then any local array modification below need
        // to be removed, to avoid array merges.
        // TODO: This should apply to any modifications of compound types, not just arrays.
        // TODO It would probably be better for these modifiers to fail rather than be removed, but at the
        // moment, I don't think modifiers can currently fail/recover based on the presence of other modifiers.
        for (const auto& modifier : targetNode->getEdits().modifierRange(m_modifierToAdd->m_targetPath)) {
            if (m_modifierToAdd->m_targetPath.isStrictPrefixOf(modifier->getTargetPath()) &&
                modifier->as<ArraySizeModifier>()) {
                subcommands.emplace_back(std::make_unique<RemoveModifierCommand>(
                    "Remove modifier subcommand", m_targetNodeId, modifier->getTargetPath()));
            }
        }
        for (auto it = subcommands.rbegin(); it != subcommands.rend(); ++it) {
            addSubCommand(std::move(*it));
        }
    }

    if (const Modifier* const modifier = targetNode->findModifier(m_modifierToAdd->m_targetPath)) {
        addSubCommand(std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", m_targetNodeId,
                                                              m_modifierToAdd->m_targetPath));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.addModifier(m_targetNodeId, *m_modifierToAdd);

    return true;
}

void babelwires::AddConnectionCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.addModifier(m_targetNodeId, *m_modifierToAdd);
}

void babelwires::AddConnectionCommand::undo(Project& project) const {
    project.removeModifier(m_targetNodeId, m_modifierToAdd->m_targetPath);
    CompoundCommand::undo(project);
}
