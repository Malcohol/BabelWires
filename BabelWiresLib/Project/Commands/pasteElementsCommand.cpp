/**
 * The command which pastes content into a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/Commands/pasteElementsCommand.hpp"

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/projectData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"

#include <cassert>
#include <unordered_set>

babelwires::PasteElementsCommand::PasteElementsCommand(std::string commandName, ProjectData dataToPaste)
    : SimpleCommand(std::move(commandName))
    , m_dataToPaste(std::move(dataToPaste)) {}

bool babelwires::PasteElementsCommand::initialize(const Project& project) {
    std::unordered_map<ElementId, ElementId> remappingTable;
    {
        std::vector<ElementId> originalIds;
        for (const auto& element : m_dataToPaste.m_elements) {
            originalIds.emplace_back(element->m_id);
        }
        std::vector<ElementId> availableIds = originalIds;
        project.updateWithAvailableIds(availableIds);
        assert((availableIds.size() == originalIds.size()) && "The array should not change size");
        auto oit = originalIds.begin();
        auto ait = availableIds.begin();
        for (; oit != originalIds.end(); ++oit, ++ait) {
            remappingTable.insert(std::pair(*oit, *ait));
        }
    }
    // We preserve in-connections from outside the set of elements only if we're
    // pasting into the "same" project.
    const bool preserveInConnections = (project.getProjectId() == m_dataToPaste.m_projectId);

    for (auto& element : m_dataToPaste.m_elements) {
        auto it = remappingTable.find(element->m_id);
        assert((it != remappingTable.end()) && "All element ids should be in the map");
        ElementId newElementId = it->second;
        element->m_id = newElementId;

        auto newEnd = std::remove_if(
            element->m_modifiers.begin(), element->m_modifiers.end(),
            [this, &remappingTable, preserveInConnections, newElementId,
             &project](std::unique_ptr<ModifierData>& modData) {
                if (auto* assignFromData = modData.get()->as<ConnectionModifierData>()) {
                    ElementId& sourceId = assignFromData->m_sourceId;
                    auto it = remappingTable.find(sourceId);
                    if (it == remappingTable.end()) {
                        if (!preserveInConnections || !project.getFeatureElement(sourceId)) {
                            // Discard it if we're not preserving in-connections OR the source doesn't exist.
                            return true;
                        }
                    } else {
                        // Update the id.
                        sourceId = it->second;
                    }
                    m_connectionsToPaste.emplace_back(ConnectionDescription(newElementId, *assignFromData));
                    return true;
                }
                return false;
            });
        element->m_modifiers.erase(newEnd, element->m_modifiers.end());
    }
    return true;
}

void babelwires::PasteElementsCommand::execute(Project& project) const {
    std::unordered_set<UiPosition> occupiedPositions;

    // Find unoccupied UI positions.
    for (const auto& [_, e] : project.getElements()) {
        occupiedPositions.insert(e->getUiPosition());
    }

    babelwires::UiCoord offset = 0;

    // After 50 attempts, allow duplication locations.
    for (int i = 0; i < 50; ++i) {
        babelwires::UiCoord attemptedOffset = offset;
        for (const auto& elementData : m_dataToPaste.m_elements) {
            UiPosition pos = elementData->m_uiData.m_uiPosition + offset;
            if (occupiedPositions.find(pos) != occupiedPositions.end()) {
                offset += 30;
                break;
            }
        }
        if (attemptedOffset == offset) {
            break;
        }
    }

    for (const auto& elementData : m_dataToPaste.m_elements) {
        // TODO
        std::unique_ptr<ElementData> clone = elementData->clone();
        clone->m_uiData.m_uiPosition += offset;
        project.addFeatureElement(*clone);
    }
    for (const auto& connection : m_connectionsToPaste) {
        auto newModifier = std::make_unique<ConnectionModifierData>();
        newModifier->m_pathToFeature = connection.m_pathToTargetFeature;
        newModifier->m_sourceId = connection.m_sourceId;
        newModifier->m_pathToSourceFeature = connection.m_pathToSourceFeature;
        project.addModifier(connection.m_targetId, *newModifier);
    }
}

void babelwires::PasteElementsCommand::undo(Project& project) const {
    for (const auto& connection : m_connectionsToPaste) {
        project.removeModifier(connection.m_targetId, connection.m_pathToTargetFeature);
    }
    for (const auto& elementData : m_dataToPaste.m_elements) {
        project.removeElement(elementData->m_id);
    }
}
