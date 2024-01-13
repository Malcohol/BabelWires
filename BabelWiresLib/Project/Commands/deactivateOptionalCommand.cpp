/**
 * The command which deactivates an optional in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::DeactivateOptionalCommand::DeactivateOptionalCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               ShortId optional)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_optional(optional)
{
}

bool babelwires::DeactivateOptionalCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    const auto [compoundFeature, isActivated] =
        ValueFeatureHelper::getInfoFromRecordWithOptionalsFeature(m_pathToRecord.tryFollow(*inputFeature), m_optional);

    if (!compoundFeature) {
        return false;   
    }

    if (!isActivated) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->getEdits().findModifier(m_pathToRecord)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            m_wasModifier = true;
        }
    }

    FeaturePath pathToOptional = m_pathToRecord;
    pathToOptional.pushStep(PathStep(m_optional));
    addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_elementId, pathToOptional));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.deactivateOptional(m_elementId, m_pathToRecord, m_optional, true);

    return true;
}

void babelwires::DeactivateOptionalCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.deactivateOptional(m_elementId, m_pathToRecord, m_optional, true);
}

void babelwires::DeactivateOptionalCommand::undo(Project& project) const {
    project.activateOptional(m_elementId, m_pathToRecord, m_optional, m_wasModifier);
    CompoundCommand::undo(project);
}
