/**
 * The command which adds a modifier to a feature element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Project/Commands/addModifierCommand.hpp"

#include "BabelWiresLib/Features/rootFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

babelwires::AddModifierCommand::AddModifierCommand(std::string commandName, ElementId targetId,
                                                   std::unique_ptr<ModifierData> modifierToAdd)
    : SimpleCommand(commandName)
    , m_targetElementId(targetId)
    , m_modifierToAdd(std::move(modifierToAdd)) {}

bool babelwires::AddModifierCommand::initialize(const Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_targetElementId);

    if (!element) {
        return false;
    }

    const Feature* const inputFeature = element->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    if (!m_modifierToAdd->m_pathToFeature.tryFollow(*inputFeature)) {
        return false;
    }

    if (const Modifier* modifier = element->findModifier(m_modifierToAdd->m_pathToFeature)) {
        m_modifierToRemove = modifier->getModifierData().clone();
    }
    return true;
}

void babelwires::AddModifierCommand::execute(Project& project) const {
    if (m_modifierToRemove) {
        project.removeModifier(m_targetElementId, m_modifierToRemove->m_pathToFeature);
    }
    project.addModifier(m_targetElementId, *m_modifierToAdd);
}

void babelwires::AddModifierCommand::undo(Project& project) const {
    project.removeModifier(m_targetElementId, m_modifierToAdd->m_pathToFeature);
    if (m_modifierToRemove) {
        project.addModifier(m_targetElementId, *m_modifierToRemove);
    }
}
