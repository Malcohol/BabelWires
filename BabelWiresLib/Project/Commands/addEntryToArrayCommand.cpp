/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Project/Commands/addEntryToArrayCommand.hpp"

#include "BabelWiresLib/Features/arrayFeature.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include <cassert>

babelwires::AddEntryToArrayCommand::AddEntryToArrayCommand(std::string commandName, ElementId elementId,
                                                           FeaturePath featurePath, unsigned int indexOfNewEntry)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfNewEntry(indexOfNewEntry) {}

bool babelwires::AddEntryToArrayCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->as<const ArrayFeature>();
    if (!arrayFeature) {
        return false;
    }

    if (!arrayFeature->getSizeRange().contains(arrayFeature->getNumFeatures() + 1)) {
        return false;
    }

    if (m_indexOfNewEntry > arrayFeature->getNumFeatures()) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->findModifier(m_pathToArray)) {
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    return true;
}

void babelwires::AddEntryToArrayCommand::execute(Project& project) const {
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntry, 1, true);
}

void babelwires::AddEntryToArrayCommand::undo(Project& project) const {
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntry, 1, m_wasModifier);
}
