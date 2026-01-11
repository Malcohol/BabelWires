/**
 * The command which activates optionals in a RecordType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

babelwires::ActivateOptionalCommand::ActivateOptionalCommand(std::string commandName, NodeId nodeId, Path pathToRecord,
                                                             ShortId optional)
    : SimpleCommand(commandName)
    , m_nodeId(nodeId)
    , m_pathToRecord(std::move(pathToRecord))
    , m_optional(optional) {}

bool babelwires::ActivateOptionalCommand::initialize(const Project& project) {
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

    if (it->second) {
        return false;
    }

    if (const Modifier* modifier = nodeToModify->findModifier(m_pathToRecord)) {
        if (const auto* activateOptionalsModifierData = modifier->getModifierData().tryAs<SelectOptionalsModifierData>()) {
            auto activatedOptionals = activateOptionalsModifierData->getOptionalActivationData();
            auto ait = activatedOptionals.find(m_optional);
            if (ait != activatedOptionals.end()) {
                if (!ait->second) {
                    m_wasDeactivated = true;
                } else {
                    // Already activated.
                    return false;
                }
            }
        }
    }

    return true;
}

void babelwires::ActivateOptionalCommand::execute(Project& project) const {
    project.setOptionalActivation(m_nodeId, m_pathToRecord, m_optional, true);
}

void babelwires::ActivateOptionalCommand::undo(Project& project) const {
    if (m_wasDeactivated) {
        project.setOptionalActivation(m_nodeId, m_pathToRecord, m_optional, false);
    } else {
        project.resetOptionalActivation(m_nodeId, m_pathToRecord, m_optional);
    }
}
