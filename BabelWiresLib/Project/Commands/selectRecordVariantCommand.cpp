/**
 * The command which selects the variant in a RecordWithVariantType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/selectRecordVariantCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SelectRecordVariantCommand::SelectRecordVariantCommand(std::string commandName, NodeId elementId,
                                                               Path featurePath, ShortId tagToSelect)
    : CompoundCommand(commandName)
    , m_nodeId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_tagToSelect(tagToSelect) {}

babelwires::SelectRecordVariantCommand::~SelectRecordVariantCommand() = default;

bool babelwires::SelectRecordVariantCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);
    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    const auto [compoundFeature, isCurrentTag, fieldsToRemove] =
        ValueTreeHelper::getInfoFromRecordWithVariantsFeature(m_pathToRecord.tryFollow(*input), m_tagToSelect);

    if (!compoundFeature) {
        return false;   
    }

    if (isCurrentTag) {
        return false;
    }

    if (const Modifier* const modifier = nodeToModify->findModifier(m_pathToRecord)) {
        m_recordModifierToRemove = modifier->getModifierData().clone();
    }

    for (const auto& field : fieldsToRemove) {
        Path pathToField = m_pathToRecord;
        pathToField.pushStep(PathStep(field));
        addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, pathToField));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    if (m_recordModifierToRemove) {
        project.removeModifier(m_nodeId, m_pathToRecord);
    }

    SelectRecordVariantModifierData modifierToAdd;
    modifierToAdd.m_targetPath = m_pathToRecord;
    modifierToAdd.m_tagToSelect = m_tagToSelect;
    m_recordModifierToAdd = std::make_unique<SelectRecordVariantModifierData>(std::move(modifierToAdd));

    project.addModifier(m_nodeId, *m_recordModifierToAdd);

    return true;
}

void babelwires::SelectRecordVariantCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    if (m_recordModifierToRemove) {
        project.removeModifier(m_nodeId, m_pathToRecord);
    }
    project.addModifier(m_nodeId, *m_recordModifierToAdd);
}

void babelwires::SelectRecordVariantCommand::undo(Project& project) const {
    project.removeModifier(m_nodeId, m_pathToRecord);
    if (m_recordModifierToRemove) {
        project.addModifier(m_nodeId, *m_recordModifierToRemove);
    }
    CompoundCommand::undo(project);
}
