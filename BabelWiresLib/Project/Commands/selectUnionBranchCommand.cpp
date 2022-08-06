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
#include <BabelWiresLib/Features/rootFeature.hpp>

babelwires::SelectUnionBranchCommand::SelectUnionBranchCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               Identifier tag)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToUnion(std::move(featurePath))
    , m_newTag(tag)
{
}

bool babelwires::SelectUnionBranchCommand::initialize(const Project& project) {
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

    if (!unionFeature->isTag(m_newTag)) {
        return false;
    }

    if (m_newTag == unionFeature->getSelectedTag()) {
        return false;
    }

    if (const Modifier *const modifier = elementToModify->findModifier(m_pathToUnion)) {
        m_oldModifier = modifier->getModifierData().clone();
    }

    return true;
}

void babelwires::SelectUnionBranchCommand::execute(Project& project) const {
    // TODO
}

void babelwires::SelectUnionBranchCommand::undo(Project& project) const {
    // TODO
}
