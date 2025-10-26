/**
 * Commands which removes modifiers from a node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>

#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <cassert>

babelwires::RemoveModifierCommand::RemoveModifierCommand(std::string commandName, NodeId targetId, Path featurePath)
    : CompoundCommand(std::move(commandName))
    , m_nodeId(targetId)
    , m_path(featurePath) {}

bool babelwires::RemoveModifierCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);

    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    auto* modifier = nodeToModify->getEdits().findModifier(m_path);
    if (!modifier) {
        return false;
    }

    // If the user is removing the outermost connection at compound then the structure will simply go back to default.
    // We can assume the user knows what they are doing and we can adjust affected modifiers automatically.
    // However, in a complex graph of connections, the modifiers in a compound might not adapt correctly to
    // the newly discovered structure from the ancestor connection. In that case, we'll just let them fail
    // so the user is informed.
    bool hasAncestorConnection = false;
    for (auto connectionModifier : nodeToModify->getConnectionModifiers()) {
        if (connectionModifier->getTargetPath().isStrictPrefixOf(m_path)) {
            hasAncestorConnection = true;
            break;
        }
    }

    if (!hasAncestorConnection) {
        // TODO: There should be a way to move this to a virtual function on modifiers, so these modifiers know how to
        // remove themselves cleanly.
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            auto [compoundFeature, currentSize, range, initialSize] =
                ValueTreeHelper::getInfoFromArray(tryFollowPath(m_path, *input));
            if (compoundFeature) {
                if (currentSize != initialSize) {
                    addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(m_nodeId, m_path, initialSize,
                                                                                     initialSize - currentSize));
                }
            }
        } else if (const auto* optModifierData = modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            auto [compoundFeature, optionals] =
                ValueTreeHelper::getInfoFromRecordWithOptionals(tryFollowPath(m_path, *input));
            if (compoundFeature) {
                for (auto activationState : optModifierData->getOptionalActivationData()) {
                    if (activationState.second) {
                        Path pathToOptional = m_path;
                        pathToOptional.pushStep(activationState.first);
                        addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, pathToOptional));
                    } else {
                        // No data to clean up for deactivated optionals.
                    }
                }
            }
        } else if (const auto* varModifierData = modifier->getModifierData().as<SelectRecordVariantModifierData>()) {
            auto [compoundFeature, isDefault, fieldsToRemove] =
                ValueTreeHelper::getInfoFromRecordWithVariants(tryFollowPath(m_path, *input));
            if (!isDefault) {
                for (auto fieldToRemove : fieldsToRemove) {
                    Path pathToFieldToRemove = m_path;
                    pathToFieldToRemove.pushStep(fieldToRemove);
                    addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, pathToFieldToRemove));
                }
            }
        }
    }

    addSubCommand(std::make_unique<RemoveSimpleModifierSubcommand>(m_nodeId, m_path));

    return CompoundCommand::initializeAndExecute(project);
}
