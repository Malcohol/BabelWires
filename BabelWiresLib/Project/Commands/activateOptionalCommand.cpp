/**
 * The command which activates optionals in a RecordType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::ActivateOptionalCommand::ActivateOptionalCommand(std::string commandName, NodeId elementId, Path featurePath,
                               ShortId optional)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToRecord(std::move(featurePath))
    , m_optional(optional)
{
}

bool babelwires::ActivateOptionalCommand::initialize(const Project& project) {
    const Node* nodeToModify = project.getNode(m_elementId);
    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    const auto [compoundFeature, optionals] =
        ValueTreeHelper::getInfoFromRecordWithOptionalsFeature(m_pathToRecord.tryFollow(*input));

    if (!compoundFeature) {
        return false;   
    }

    auto it = optionals.find(m_optional);
    
    if (it == optionals.end()) {
        return false;
    }

    if (it->second) {
        return false;
    }

    if (const Modifier* modifier = nodeToModify->findModifier(m_pathToRecord)) {
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
