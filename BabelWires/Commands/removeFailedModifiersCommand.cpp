/**
 * The command which removes all failed modifiers at or beneath the given path.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Commands/removeFailedModifiersCommand.hpp"

#include "BabelWires/Commands/removeModifierCommand.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/project.hpp"

babelwires::RemoveFailedModifiersCommand::RemoveFailedModifiersCommand(std::string commandName, ElementId targetId,
                                                                       FeaturePath featurePath)
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
            addSubCommand(std::make_unique<RemoveSimpleModifierCommand>("Remove failed modifier", m_targetId,
                                                                        modifier->getPathToFeature()));
            ++numFailedModifiers;
        }
    }

    if (numFailedModifiers == 0) {
        return false;
    }

    return CompoundCommand::initializeAndExecute(project);
}
