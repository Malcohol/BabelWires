/**
 * The command which removes all failed modifiers at or beneath the given path.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>

#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::RemoveFailedModifiersCommand::RemoveFailedModifiersCommand(std::string commandName, NodeId targetId,
                                                                       Path featurePath)
    : CompoundCommand(commandName)
    , m_targetId(targetId)
    , m_targetPath(featurePath) {}

bool babelwires::RemoveFailedModifiersCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_targetId);

    if (!nodeToModify) {
        return false;
    }

    int numFailedModifiers = 0;
    for (const Modifier* modifier : nodeToModify->getEdits().modifierRange(m_targetPath)) {
        if (modifier->isFailed()) {
            addSubCommand(std::make_unique<RemoveSimpleModifierSubcommand>(m_targetId,
                                                                        modifier->getTargetPath()));
            ++numFailedModifiers;
        }
    }

    if (numFailedModifiers == 0) {
        return false;
    }

    return CompoundCommand::initializeAndExecute(project);
}
