/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::AddEntriesToArrayCommand::AddEntriesToArrayCommand(std::string commandName, ElementId elementId,
                                                               FeaturePath featurePath, unsigned int indexOfNewEntries,
                                                               unsigned int numEntriesToAdd)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfNewEntries(indexOfNewEntries)
    , m_numEntriesToAdd(numEntriesToAdd) {}

bool babelwires::AddEntriesToArrayCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    const auto [compoundFeature, currentSize, range, initialSize] =
        ValueFeatureHelper::getInfoFromArrayFeature(m_pathToArray.tryFollow(*inputFeature));
    if (!compoundFeature) {
        return false;
    }

    if (!range.contains(currentSize + m_numEntriesToAdd)) {
        return false;
    }

    if (m_indexOfNewEntries > currentSize) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->findModifier(m_pathToArray)) {
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(m_elementId, m_pathToArray, m_indexOfNewEntries,
                                                                     m_numEntriesToAdd));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntries, m_numEntriesToAdd, true);

    return true;
}

void babelwires::AddEntriesToArrayCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntries, m_numEntriesToAdd, true);
}

void babelwires::AddEntriesToArrayCommand::undo(Project& project) const {
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntries, m_numEntriesToAdd, m_wasModifier);
    CompoundCommand::undo(project);
}
