/**
 * The command which activates optionals in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/selectRecordVariantCommand.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/selectUnionBranchModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SelectRecordVariantCommand::SelectRecordVariantCommand(std::string commandName, ElementId elementId,
                                                               FeaturePath featurePath, ShortId tagToSelect)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToUnion(std::move(featurePath))
    , m_tagToSelect(tagToSelect) {}

babelwires::SelectRecordVariantCommand::~SelectRecordVariantCommand() = default;

bool babelwires::SelectRecordVariantCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    const auto [compoundFeature, isCurrentTag, fieldsToRemove] =
        ValueFeatureHelper::getInfoFromRecordWithVariantsFeature(m_pathToUnion.tryFollow(*inputFeature), m_tagToSelect);

    if (!compoundFeature) {
        return false;   
    }

    if (isCurrentTag) {
        return false;
    }

    if (const Modifier* const modifier = elementToModify->findModifier(m_pathToUnion)) {
        m_unionModifierToRemove = modifier->getModifierData().clone();
    }

    for (const auto& field : fieldsToRemove) {
        FeaturePath pathToField = m_pathToUnion;
        pathToField.pushStep(PathStep(field));
        addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_elementId, pathToField));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    if (m_unionModifierToRemove) {
        project.removeModifier(m_elementId, m_pathToUnion);
    }

    SelectUnionBranchModifierData modifierToAdd;
    modifierToAdd.m_pathToFeature = m_pathToUnion;
    modifierToAdd.m_tagToSelect = m_tagToSelect;
    m_unionModifierToAdd = std::make_unique<SelectUnionBranchModifierData>(std::move(modifierToAdd));

    project.addModifier(m_elementId, *m_unionModifierToAdd);

    return true;
}

void babelwires::SelectRecordVariantCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    if (m_unionModifierToRemove) {
        project.removeModifier(m_elementId, m_pathToUnion);
    }
    project.addModifier(m_elementId, *m_unionModifierToAdd);
}

void babelwires::SelectRecordVariantCommand::undo(Project& project) const {
    project.removeModifier(m_elementId, m_pathToUnion);
    if (m_unionModifierToRemove) {
        project.addModifier(m_elementId, *m_unionModifierToRemove);
    }
    CompoundCommand::undo(project);
}
