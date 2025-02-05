/**
 * The command which deactivates an optional in a RecordType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::DeactivateOptionalCommand::DeactivateOptionalCommand(std::string commandName, NodeId elementId, Path featurePath,
                               ShortId optional)
    : CompoundCommand(commandName)
    , m_nodeId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_optional(optional)
{
}

bool babelwires::DeactivateOptionalCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);
    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    const auto [compoundFeature, optionals] =
        ValueTreeHelper::getInfoFromRecordWithOptionals(m_pathToRecord.tryFollow(*input));

    if (!compoundFeature) {
        return false;   
    }
    auto it = optionals.find(m_optional);
    
    if (it == optionals.end()) {
        return false;
    }

    if (!it->second) {
        return false;
    }

    if (const Modifier* modifier = nodeToModify->getEdits().findModifier(m_pathToRecord)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            m_wasModifier = true;
        }
    }

    Path pathToOptional = m_pathToRecord;
    pathToOptional.pushStep(PathStep(m_optional));
    addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, pathToOptional));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.deactivateOptional(m_nodeId, m_pathToRecord, m_optional, true);

    return true;
}

void babelwires::DeactivateOptionalCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.deactivateOptional(m_nodeId, m_pathToRecord, m_optional, true);
}

void babelwires::DeactivateOptionalCommand::undo(Project& project) const {
    project.activateOptional(m_nodeId, m_pathToRecord, m_optional, m_wasModifier);
    CompoundCommand::undo(project);
}
