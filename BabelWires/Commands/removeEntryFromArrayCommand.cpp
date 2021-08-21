/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Commands/removeEntryFromArrayCommand.hpp"

#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWires/Commands/removeAllEditsCommand.hpp"
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::RemoveEntryFromArrayCommand::RemoveEntryFromArrayCommand(std::string commandName, ElementId elementId,
                                                                     FeaturePath featurePath, int indexOfEntryToRemove,
                                                                     int numEntriesToRemove)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfEntryToRemove(indexOfEntryToRemove)
    , m_numEntriesToRemove(numEntriesToRemove) {
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

    auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->asA<const ArrayFeature>();
    if (!arrayFeature) {
        return false;
    }

    if (!arrayFeature->getSizeRange().contains(arrayFeature->getNumFeatures() - m_numEntriesToRemove)) {
        return false;
    }

    if (m_indexOfEntryToRemove < 0) {
        m_indexOfEntryToRemove = arrayFeature->getNumFeatures() - m_numEntriesToRemove;
    }

    if (arrayFeature->getNumFeatures() <= m_indexOfEntryToRemove) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->getEdits().findModifier(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().asA<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    for (int i = 0; i < m_numEntriesToRemove; ++i) {
        FeaturePath p = m_pathToArray;
        p.pushStep(PathStep(m_indexOfEntryToRemove + i));
        addSubCommand(std::make_unique<RemoveAllEditsCommand>("Remove array entry subcommand", m_elementId, p));
    }
    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, true);
    return true;
}

void babelwires::RemoveEntryFromArrayCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, true);
}

void babelwires::RemoveEntryFromArrayCommand::undo(Project& project) const {
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, m_wasModifier);
    CompoundCommand::undo(project);
}
