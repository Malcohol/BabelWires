/**
 * The command which activates optionals in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Commands/activateOptionalCommand.hpp"

#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/localModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

babelwires::ActivateOptionalCommand::ActivateOptionalCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               Identifier optional)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_optional(optional)
{
}

bool babelwires::ActivateOptionalCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto recordFeature = m_pathToRecord.tryFollow(*inputFeature)->as<const RecordWithOptionalsFeature>();
    if (!recordFeature) {
        return false;
    }

    if (!recordFeature->isOptional(m_optional)) {
        return false;
    }

    if (recordFeature->isActivated(m_optional)) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->findModifier(m_pathToRecord)) {
        if (modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            m_wasModifier = true;
        }
    }

    return true;
}

void babelwires::ActivateOptionalCommand::execute(Project& project) const {
    project.activateOptional(m_elementId, m_pathToRecord, m_optional, true);
}

void babelwires::ActivateOptionalCommand::undo(Project& project) const {
    project.deactivateOptional(m_elementId, m_pathToRecord, m_optional, m_wasModifier);
}
