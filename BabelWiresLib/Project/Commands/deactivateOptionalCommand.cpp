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
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

babelwires::DeactivateOptionalCommand::DeactivateOptionalCommand(std::string commandName, NodeId nodeId, Path featurePath,
                               ShortId optional)
    : CompoundCommand(commandName)
    , m_nodeId(nodeId)
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
        ValueTreeHelper::getInfoFromRecordWithOptionals(tryFollowPath(m_pathToRecord, *input));

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
        if (const auto* activateOptionalsModifierData = modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            auto activatedOptionals = activateOptionalsModifierData->getOptionalActivationData();
            auto ait = activatedOptionals.find(m_optional);
            if (ait != activatedOptionals.end()) {
                if (ait->second) {
                    m_wasActivated = true;
                } else {
                    // Already deactivated.
                    return false;
                }
            }
        }
    }

    Path pathToOptional = m_pathToRecord;
    pathToOptional.pushStep(m_optional);
    addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, pathToOptional));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.setOptionalActivation(m_nodeId, m_pathToRecord, m_optional, false);

    return true;
}

void babelwires::DeactivateOptionalCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.setOptionalActivation(m_nodeId, m_pathToRecord, m_optional, false);
}

void babelwires::DeactivateOptionalCommand::undo(Project& project) const {
    if (m_wasActivated) {
        project.setOptionalActivation(m_nodeId, m_pathToRecord, m_optional, true);
    } else {
        project.resetOptionalActivation(m_nodeId, m_pathToRecord, m_optional);
    }
    CompoundCommand::undo(project);
}
