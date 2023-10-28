/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::AddEntriesToArrayCommand::AddEntriesToArrayCommand(std::string commandName, ElementId elementId,
                                                           FeaturePath featurePath, unsigned int indexOfNewEntries, unsigned int numEntriesToAdd)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfNewEntries(indexOfNewEntries)
    , m_numEntriesToAdd(numEntriesToAdd) {}

bool babelwires::AddEntriesToArrayCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto [compoundFeature, currentSize, range, initialSize] = ValueFeatureHelper::getInfoFromArrayFeature(m_pathToArray.tryFollow(*inputFeature));
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

    m_indirectlyAffectedArrays = projectUtilities::getAllDerivedValues(project, m_elementId, m_pathToArray);

    return true;
}

void babelwires::AddEntriesToArrayCommand::execute(Project& project) const {
    for (auto a : m_indirectlyAffectedArrays) {
        const ElementId elementId = std::get<0>(a);
        const FeaturePath& pathToArray = std::get<1>(a);
        project.adjustModifiersInArrayElements(elementId, pathToArray, m_indexOfNewEntries, m_numEntriesToAdd);
    }
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntries, m_numEntriesToAdd, true);
}

void babelwires::AddEntriesToArrayCommand::undo(Project& project) const {
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfNewEntries, m_numEntriesToAdd, m_wasModifier);
    for (auto it = m_indirectlyAffectedArrays.rbegin(); it != m_indirectlyAffectedArrays.rend(); ++it) {
        const ElementId elementId = std::get<0>(*it);
        const FeaturePath& pathToArray = std::get<1>(*it);
        project.adjustModifiersInArrayElements(elementId, pathToArray, m_indexOfNewEntries, -m_numEntriesToAdd);
    }
}
