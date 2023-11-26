/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::RemoveEntryFromArrayCommand::RemoveEntryFromArrayCommand(std::string commandName, ElementId elementId,
                                                                     FeaturePath featurePath,
                                                                     unsigned int indexOfEntryToRemove,
                                                                     unsigned int numEntriesToRemove)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfEntryToRemove(indexOfEntryToRemove)
    , m_numEntriesToRemove(numEntriesToRemove)
    , m_isSubcommand(false) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
}

bool babelwires::RemoveEntryFromArrayCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->as<const ArrayFeature>();

    int currentSize = -1;
    unsigned int minimumSize = 0;
    if (auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->as<ArrayFeature>()) {
        currentSize = arrayFeature->getNumFeatures();
        minimumSize = arrayFeature->getSizeRange().m_min;
    } else if (auto valueFeature = m_pathToArray.tryFollow(*inputFeature)->as<ValueFeature>()) {
        if (auto arrayType = valueFeature->getType().as<ArrayType>()) {
            currentSize = arrayType->getNumChildren(valueFeature->getValue());
            minimumSize = arrayType->getSizeRange().m_min;
        }
    }
    if (currentSize < 0) {
        return false;
    }

    if (currentSize - m_numEntriesToRemove < minimumSize) {
        return false;
    }

    if (m_indexOfEntryToRemove > currentSize - 1) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->getEdits().findModifier(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(m_elementId, m_pathToArray, m_indexOfEntryToRemove, -m_numEntriesToRemove));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, !m_isSubcommand);
    return true;
}

void babelwires::RemoveEntryFromArrayCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, !m_isSubcommand);
}

void babelwires::RemoveEntryFromArrayCommand::undo(Project& project) const {
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, m_wasModifier);
    CompoundCommand::undo(project);
}
