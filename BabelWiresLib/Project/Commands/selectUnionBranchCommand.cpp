/**
 * The command which activates optionals in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/selectUnionBranchCommand.hpp>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/selectUnionBranchModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Commands/removeAllEditsCommand.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

babelwires::SelectUnionBranchCommand::SelectUnionBranchCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               Identifier tagToSelect)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToUnion(std::move(featurePath))
    , m_tagToSelect(tagToSelect)
{
}

bool babelwires::SelectUnionBranchCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto unionFeature = m_pathToUnion.tryFollow(*inputFeature)->as<const UnionFeature>();
    if (!unionFeature) {
        return false;
    }

    if (!unionFeature->isTag(m_tagToSelect)) {
        return false;
    }

    if (m_tagToSelect == unionFeature->getSelectedTag()) {
        return false;
    }

    if (const Modifier *const modifier = elementToModify->findModifier(m_pathToUnion)) {
        m_unionModifierToRemove = modifier->getModifierData().clone();
    }

    for (const auto& field : unionFeature->getFieldsOfSelectedBranch()) {
        FeaturePath pathToField = m_pathToUnion;
        pathToField.pushStep(PathStep(field));
        addSubCommand(std::make_unique<RemoveAllEditsCommand>("Remove union branch subcommand", m_elementId, pathToField));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    if (m_unionModifierToRemove) {
        project.removeModifier(m_elementId, m_pathToUnion);
    }

    SelectUnionBranchModifierData modifierToAdd;
    modifierToAdd.m_pathToFeature = m_pathToUnion;
    modifierToAdd.m_tag = m_tagToSelect;
    m_unionModifierToAdd = std::make_unique<SelectUnionBranchModifierData>(std::move(modifierToAdd));
    
    project.addModifier(m_elementId, *m_unionModifierToAdd);

    return true;
}

void babelwires::SelectUnionBranchCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    if (m_unionModifierToRemove) {
        project.removeModifier(m_elementId, m_pathToUnion);
    }
    project.addModifier(m_elementId, *m_unionModifierToAdd);
}

void babelwires::SelectUnionBranchCommand::undo(Project& project) const {
    project.removeModifier(m_elementId, m_pathToUnion);
    if (m_unionModifierToRemove) {
        project.addModifier(m_elementId, *m_unionModifierToRemove);
    }
    CompoundCommand::undo(project);
}
