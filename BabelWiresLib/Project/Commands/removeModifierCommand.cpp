/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>
#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::RemoveModifierCommand::RemoveModifierCommand(std::string commandName, ElementId targetId,
                                                         Path featurePath)
    : CompoundCommand(std::move(commandName))
    , m_elementId(targetId)
    , m_path(featurePath) {}

bool babelwires::RemoveModifierCommand::initializeAndExecute(Project& project) {
    const Node* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    const ValueTreeNode* const input = elementToModify->getInput();
    if (!input) {
        return false;
    }

    auto* modifier = elementToModify->getEdits().findModifier(m_path);
    if (!modifier) {
        return false;
    }

    // If the user is removing the outermost connection at compound then the structure will simply go back to default.
    // We can assume the user knows what they are doing and we can adjust affected modifiers automatically.
    // However, in a complex graph of connections, the modifiers in a compound might not adapt correctly to
    // the newly discovered structure from the ancestor connection. In that case, we'll just let them fail
    // so the user is informed.
    bool hasAncestorConnection = false;
    for (auto connectionModifier : elementToModify->getConnectionModifiers()) {
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
                ValueTreeHelper::getInfoFromArrayFeature(m_path.tryFollow(*input));
            if (compoundFeature) {
                if (currentSize != initialSize) {
                    addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(
                        m_elementId, m_path, initialSize, initialSize - currentSize));
                }
            }
        } else if (const auto* optModifierData = modifier->getModifierData().as<ActivateOptionalsModifierData>()) {
            auto [compoundFeature, optionals] =
                ValueTreeHelper::getInfoFromRecordWithOptionalsFeature(m_path.tryFollow(*input));
            if (compoundFeature) {
                for (auto optionalField : optionals) {
                    if (optionalField.second) {
                        addSubCommand(std::make_unique<DeactivateOptionalCommand>(
                            "DeactivateOptionalCommand subcommand", m_elementId, m_path, optionalField.first));
                    }
                }
            }
        } else if (const auto* varModifierData = modifier->getModifierData().as<SelectRecordVariantModifierData>()) {
            auto [compoundFeature, isDefault, fieldsToRemove] =
                ValueTreeHelper::getInfoFromRecordWithVariantsFeature(m_path.tryFollow(*input));
            if (!isDefault) {
                for (auto fieldToRemove : fieldsToRemove) {
                    Path pathToFieldToRemove = m_path;
                    pathToFieldToRemove.pushStep(babelwires::PathStep(fieldToRemove));
                    addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_elementId, pathToFieldToRemove));
                }
            }
        }
    }

    addSubCommand(std::make_unique<RemoveSimpleModifierSubcommand>(m_elementId, m_path));

    return CompoundCommand::initializeAndExecute(project);
}
