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
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::RemoveEntryFromArrayCommand::RemoveEntryFromArrayCommand(std::string commandName, ElementId elementId,
                                                                     FeaturePath featurePath, int indexOfEntryToRemove,
                                                                     int numEntriesToRemove)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_indexOfEntryToRemove(indexOfEntryToRemove)
    , m_numEntriesToRemove(numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
}

bool babelwires::RemoveEntryFromArrayCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto arrayFeature = dynamic_cast<const ArrayFeature*>(m_pathToArray.tryFollow(*inputFeature));
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

    for (const auto& modifier : elementToModify->getEdits().modifierRange(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        const FeaturePath& modifierPath = modifierData.m_pathToFeature;
        if (m_pathToArray == modifierPath) {
            if (dynamic_cast<const ArrayInitializationData*>(&modifier->getModifierData())) {
                m_wasModifier = true;
            }
        } else {
            const unsigned int pathIndexOfStepIntoArray = m_pathToArray.getNumSteps();
            const PathStep& stepIntoArray = modifierPath.getStep(pathIndexOfStepIntoArray);
            // TODO failure?
            const ArrayIndex arrayIndex = stepIntoArray.getIndex();
            if ((arrayIndex >= m_indexOfEntryToRemove) &&
                (arrayIndex < m_indexOfEntryToRemove + m_numEntriesToRemove)) {
                m_modifiersRemovedFromEntry.emplace_back(modifierData.clone());
            }
        }
    }

    {
        const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        auto it = connectionInfo.m_requiredFor.find(elementToModify);
        if (it != connectionInfo.m_requiredFor.end()) {
            for (auto&& connection : it->second) {
                const ConnectionModifier* const cmod = std::get<0>(connection);
                const AssignFromFeatureData& modifierData = cmod->getModifierData();
                const FeaturePath& modifierPath = modifierData.m_pathToSourceFeature;
                if (m_pathToArray.isStrictPrefixOf(modifierPath)) {
                    const unsigned int pathIndexOfStepIntoArray = m_pathToArray.getNumSteps();
                    const PathStep& stepIntoArray = modifierPath.getStep(pathIndexOfStepIntoArray);
                    // TODO failure?
                    const ArrayIndex arrayIndex = stepIntoArray.getIndex();
                    if ((arrayIndex >= m_indexOfEntryToRemove) &&
                        (arrayIndex < m_indexOfEntryToRemove + m_numEntriesToRemove)) {
                        const FeatureElement* const target = std::get<1>(connection);
                        m_outgoingConnections.emplace_back(OutgoingConnection{
                            modifierData.m_pathToSourceFeature, target->getElementId(), modifierData.m_pathToFeature});
                    }
                }
            }
        }
    }

    for (int i = 0; i < m_numEntriesToRemove; ++i) {
        FeaturePath pathToEntry = m_pathToArray;
        pathToEntry.pushStep(PathStep(m_indexOfEntryToRemove + i));
        const auto pathsInThisEntry = elementToModify->getEdits().getAllExpandedPaths(pathToEntry);
        m_expandedPathsInRemovedEntries.insert(m_expandedPathsInRemovedEntries.end(), pathsInThisEntry.begin(),
                                               pathsInThisEntry.end());
    }

    return true;
}

void babelwires::RemoveEntryFromArrayCommand::execute(Project& project) const {
    for (auto&& modifierData : m_modifiersRemovedFromEntry) {
        project.removeModifier(m_elementId, modifierData->m_pathToFeature);
    }
    for (auto&& outgoingConnection : m_outgoingConnections) {
        project.removeModifier(outgoingConnection.m_targetId, outgoingConnection.m_pathInTarget);
    }
    {
        FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
        assert(elementToModify && "The element must exist");
        // This may not seem necessary, but it means we won't assert when expanding the
        // moved down entries.
        for (const auto& p : m_expandedPathsInRemovedEntries) {
            elementToModify->getEdits().setExpanded(p, false);
        }
    }
    project.removeArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, true);
}

void babelwires::RemoveEntryFromArrayCommand::undo(Project& project) const {
    project.addArrayEntries(m_elementId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, m_wasModifier);
    {
        FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
        assert(elementToModify && "The element must exist");
        for (const auto& p : m_expandedPathsInRemovedEntries) {
            elementToModify->getEdits().setExpanded(p, true);
        }
    }
    for (auto&& outgoingConnection : m_outgoingConnections) {
        AssignFromFeatureData newModifier;
        newModifier.m_pathToFeature = outgoingConnection.m_pathInTarget;
        newModifier.m_sourceId = m_elementId;
        newModifier.m_pathToSourceFeature = outgoingConnection.m_pathInSource;
        project.addModifier(outgoingConnection.m_targetId, newModifier);
    }
    for (auto&& modifierData : m_modifiersRemovedFromEntry) {
        project.addModifier(m_elementId, *modifierData);
    }
}
