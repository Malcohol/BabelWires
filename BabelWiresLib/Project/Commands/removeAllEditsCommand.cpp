/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeAllEditsCommand.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::RemoveAllEditsCommand::RemoveAllEditsCommand(std::string commandName, ElementId elementId,
                                                                     FeaturePath pathToFeatureToRemove)
    : SimpleCommand(commandName)
    , m_elementId(elementId)
    , m_pathToFeature(std::move(pathToFeatureToRemove)) {
}

bool babelwires::RemoveAllEditsCommand::initialize(const Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto featureToRemove = m_pathToFeature.tryFollow(*inputFeature);
    if (!featureToRemove) {
        return false;
    }

    for (const auto& modifier : elementToModify->getEdits().modifierRange(m_pathToFeature)) {
        const auto& modifierData = modifier->getModifierData();
        m_modifiersRemoved.emplace_back(modifierData.clone());
    }

    {
        const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        auto it = connectionInfo.m_requiredFor.find(elementToModify);
        if (it != connectionInfo.m_requiredFor.end()) {
            for (auto&& connection : it->second) {
                const ConnectionModifier* const cmod = std::get<0>(connection);
                const ConnectionModifierData& modifierData = cmod->getModifierData();
                const FeaturePath& modifierPath = modifierData.m_pathToSourceFeature;
                if (m_pathToFeature.isPrefixOf(modifierPath)) {
                    const FeatureElement* const target = std::get<1>(connection);
                    m_outgoingConnectionsRemoved.emplace_back(OutgoingConnection{
                        modifierData.m_pathToSourceFeature, target->getElementId(), modifierData.m_pathToFeature});
                }
            }
        }
    }

    const auto pathsInThisEntry = elementToModify->getEdits().getAllExpandedPaths(m_pathToFeature);
    m_expandedPathsRemoved.insert(m_expandedPathsRemoved.end(), pathsInThisEntry.begin(),
                                               pathsInThisEntry.end());

    return true;
}

void babelwires::RemoveAllEditsCommand::execute(Project& project) const {
    for (auto&& modifierData : Span<decltype(m_modifiersRemoved.rbegin())>{m_modifiersRemoved.rbegin(), m_modifiersRemoved.rend()}) {
        project.removeModifier(m_elementId, modifierData->m_pathToFeature);
    }
    for (auto&& outgoingConnection : m_outgoingConnectionsRemoved) {
        project.removeModifier(outgoingConnection.m_targetId, outgoingConnection.m_pathInTarget);
    }
    {
        FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
        assert(elementToModify && "The element must exist");
        // This may not seem necessary, but it means we won't assert when expanding the
        // moved down entries.
        for (const auto& p : m_expandedPathsRemoved) {
            elementToModify->getEdits().setExpanded(p, false);
        }
    }
}

void babelwires::RemoveAllEditsCommand::undo(Project& project) const {
    {
        FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
        assert(elementToModify && "The element must exist");
        for (const auto& p : m_expandedPathsRemoved) {
            elementToModify->getEdits().setExpanded(p, true);
        }
    }
    for (auto&& outgoingConnection : m_outgoingConnectionsRemoved) {
        ConnectionModifierData newModifier;
        newModifier.m_pathToFeature = outgoingConnection.m_pathInTarget;
        newModifier.m_sourceId = m_elementId;
        newModifier.m_pathToSourceFeature = outgoingConnection.m_pathInSource;
        project.addModifier(outgoingConnection.m_targetId, newModifier);
    }
    for (auto&& modifierData : m_modifiersRemoved) {
        project.addModifier(m_elementId, *modifierData);
    }
}
