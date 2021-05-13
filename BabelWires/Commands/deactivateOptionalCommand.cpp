/**
 * The command which deactivates an optional in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWires/Commands/deactivateOptionalCommand.hpp"

#include "BabelWires/Features/recordWithOptionalsFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/localModifier.hpp"
#include "BabelWires/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWires/Project/project.hpp"

babelwires::DeactivateOptionalCommand::DeactivateOptionalCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               FieldIdentifier optional)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_optional(optional)
{
}

bool babelwires::DeactivateOptionalCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto recordFeature = dynamic_cast<const RecordWithOptionalsFeature*>(m_pathToRecord.tryFollow(*inputFeature));
    if (!recordFeature) {
        return false;
    }

    if (!recordFeature->isOptional(m_optional)) {
        return false;
    }

    if (!recordFeature->isActivated(m_optional)) {
        return false;
    }

    return true;
}

void babelwires::DeactivateOptionalCommand::execute(Project& project) const {
    project.deactivateOptional(m_elementId, m_pathToRecord, m_optional);
}

void babelwires::DeactivateOptionalCommand::undo(Project& project) const {
    project.activateOptional(m_elementId, m_pathToRecord, m_optional);
}