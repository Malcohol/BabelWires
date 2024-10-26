/**
 * The command which adds a modifier to a feature element.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::AddModifierCommand::AddModifierCommand(std::string commandName, ElementId targetId,
                                                   std::unique_ptr<ModifierData> modifierToAdd)
    : CompoundCommand(commandName)
    , m_targetElementId(targetId)
    , m_modifierToAdd(std::move(modifierToAdd)) {}

bool babelwires::AddModifierCommand::initializeAndExecute(Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_targetElementId);

    if (!element) {
        return false;
    }

    const ValueTreeNode* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    if (!m_modifierToAdd->m_pathToFeature.tryFollow(*inputFeature)) {
        return false;
    }

    if (m_modifierToAdd->as<ConnectionModifierData>()) {
        std::vector<std::unique_ptr<Command>> subcommands;
        // If this is a connection at a compound type, then any local array modification below need
        // to be removed, to avoid array merges.
        // TODO It would probably be better for these modifiers to fail rather than be removed, but at the
        // moment, I don't think modifiers can currently fail/recover based on the presence of other modifiers.
        for (const auto& modifier : element->getEdits().modifierRange(m_modifierToAdd->m_pathToFeature)) {
            if (m_modifierToAdd->m_pathToFeature.isStrictPrefixOf(modifier->getPathToFeature()) && modifier->as<ArraySizeModifier>()) {
                subcommands.emplace_back(std::make_unique<RemoveModifierCommand>(
                    "Remove modifier subcommand", m_targetElementId, modifier->getPathToFeature()));
            }
        }
        for (auto it = subcommands.rbegin(); it != subcommands.rend(); ++it) {
            addSubCommand(std::move(*it));
        }
    }

    if (const Modifier* const modifier = element->findModifier(m_modifierToAdd->m_pathToFeature)) {
        addSubCommand(
            std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", m_targetElementId, m_modifierToAdd->m_pathToFeature));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.addModifier(m_targetElementId, *m_modifierToAdd);

    return true;
}

void babelwires::AddModifierCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.addModifier(m_targetElementId, *m_modifierToAdd);
}

void babelwires::AddModifierCommand::undo(Project& project) const {
    project.removeModifier(m_targetElementId, m_modifierToAdd->m_pathToFeature);
    CompoundCommand::undo(project);
}
