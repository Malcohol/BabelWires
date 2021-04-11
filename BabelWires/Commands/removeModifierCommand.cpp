/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Commands/removeModifierCommand.hpp"

#include "BabelWires/Commands/removeEntryFromArrayCommand.hpp"
#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::RemoveModifierCommand::RemoveModifierCommand(std::string commandName, ElementId targetId,
                                                         FeaturePath featurePath)
    : CompoundCommand(std::move(commandName))
    , m_simpleCommand(std::make_unique<RemoveSimpleModifierCommand>("SimpleCommand", targetId, featurePath)) {}

bool babelwires::RemoveModifierCommand::initializeAndExecute(Project& project) {
    ElementId elementId = m_simpleCommand->getTargetElementId();
    const FeaturePath& path = m_simpleCommand->getFeaturePath();

    const FeatureElement* elementToModify = project.getFeatureElement(elementId);

    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    if (auto arrayFeature = dynamic_cast<const ArrayFeature*>(path.tryFollow(*inputFeature))) {
        const int numEntriesToRemove = arrayFeature->getNumFeatures() - arrayFeature->getSizeRange().m_min;
        if (numEntriesToRemove > 0) {
            const int lastEntryIndex = arrayFeature->getSizeRange().m_min;
            addSubCommand(std::make_unique<RemoveEntryFromArrayCommand>("RemoveArrayEntry", elementId, path,
                                                                        lastEntryIndex, numEntriesToRemove));
        }
    }

    addSubCommand(std::move(m_simpleCommand));

    return CompoundCommand::initializeAndExecute(project);
}

babelwires::RemoveSimpleModifierCommand::RemoveSimpleModifierCommand(std::string commandName, ElementId targetId,
                                                                     FeaturePath featurePath)
    : SimpleCommand(std::move(commandName))
    , m_targetElementId(targetId)
    , m_featurePath(std::move(featurePath)) {}

bool babelwires::RemoveSimpleModifierCommand::initialize(const Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_targetElementId);
    if (!element) {
        return false;
    }

    const Modifier* modifier = element->findModifier(m_featurePath);
    if (!modifier) {
        return false;
    }

    m_modifierToRestore = modifier->getModifierData().clone();
    return true;
}

void babelwires::RemoveSimpleModifierCommand::execute(Project& project) const {
    project.removeModifier(m_targetElementId, m_featurePath);
}

void babelwires::RemoveSimpleModifierCommand::undo(Project& project) const {
    project.addModifier(m_targetElementId, *m_modifierToRestore);
}

babelwires::ElementId babelwires::RemoveSimpleModifierCommand::getTargetElementId() const {
    return m_targetElementId;
}

const babelwires::FeaturePath& babelwires::RemoveSimpleModifierCommand::getFeaturePath() const {
    return m_featurePath;
}
