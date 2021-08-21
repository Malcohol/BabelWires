/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWires/Commands/addEntryToArrayCommand.hpp"

#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::AddEntryToArrayCommand::AddEntryToArrayCommand(std::string commandName, ElementId elementId,
                                                           FeaturePath featurePath, int indexOfNewEntry)
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

    auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->asA<const ArrayFeature>();
    if (!arrayFeature) {
        return false;
    }

    if (!arrayFeature->getSizeRange().contains(arrayFeature->getNumFeatures() + 1)) {
        return false;
    }

    if (arrayFeature->getNumFeatures() <= m_indexOfNewEntry) {
        return false;
    }

    if (m_indexOfNewEntry < 0) {
        m_indexOfNewEntry = arrayFeature->getNumFeatures();
    }

    if (const Modifier* modifier = elementToModify->findModifier(m_pathToArray)) {
        if (dynamic_cast<const ArraySizeModifierData*>(&modifier->getModifierData())) {
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
