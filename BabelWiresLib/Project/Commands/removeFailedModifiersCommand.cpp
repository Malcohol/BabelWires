/**
 * The command which removes all failed modifiers at or beneath the given path.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>

#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::RemoveFailedModifiersCommand::RemoveFailedModifiersCommand(std::string commandName, ElementId targetId,
                                                                       Path featurePath)
    : CompoundCommand(commandName)
    , m_targetId(targetId)
    , m_featurePath(featurePath) {}

bool babelwires::RemoveFailedModifiersCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_targetId);

    if (!elementToModify) {
        return false;
    }

    int numFailedModifiers = 0;
    for (const Modifier* modifier : elementToModify->getEdits().modifierRange(m_featurePath)) {
        if (modifier->isFailed()) {
            addSubCommand(std::make_unique<RemoveSimpleModifierSubcommand>(m_targetId,
                                                                        modifier->getPathToModify()));
            ++numFailedModifiers;
        }
    }

    if (numFailedModifiers == 0) {
        return false;
    }

    return CompoundCommand::initializeAndExecute(project);
}
