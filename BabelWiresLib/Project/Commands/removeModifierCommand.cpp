/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/Commands/removeModifierCommand.hpp"

#include "BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp"
#include "BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp"
#include "BabelWiresLib/Features/arrayFeature.hpp"
#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

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

    auto* modifier = elementToModify->getEdits().findModifier(path);
    if (!modifier) {
        return false;
    }

    // TODO: There should be a way to move this to a virtual function on modifiers, so these modifiers know how to
    // remove themselves cleanly.
    if (modifier->getModifierData().as<ArraySizeModifierData>()) {
        if (auto arrayFeature = path.tryFollow(*inputFeature)->as<const ArrayFeature>()) {
            const int numEntriesToRemove = arrayFeature->getNumFeatures() - arrayFeature->getSizeRange().m_min;
            if (numEntriesToRemove > 0) {
                const int lastEntryIndex = arrayFeature->getSizeRange().m_min;
                addSubCommand(std::make_unique<RemoveEntryFromArrayCommand>("RemoveArrayEntry subcommand", elementId, path,
                                                                            lastEntryIndex, numEntriesToRemove));
            }
        }
    }
    else if (modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
        if (auto optionalFeature = path.tryFollow(*inputFeature)->as<const RecordWithOptionalsFeature>()) {
            for (auto optionalField : optionalFeature->getOptionalFields()) {
                if (optionalFeature->isActivated(optionalField)) {
                    addSubCommand(std::make_unique<DeactivateOptionalCommand>("DeactivateOptionalCommand subcommand", elementId, path, optionalField));
                }
            }
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
