/**
 * The command which pastes content into a project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/pasteNodesCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

#include <cassert>
#include <unordered_set>

babelwires::PasteNodesCommand::PasteNodesCommand(std::string commandName, ProjectData dataToPaste)
    : SimpleCommand(std::move(commandName))
    , m_dataToPaste(std::move(dataToPaste)) {}

bool babelwires::PasteNodesCommand::initialize(const Project& project) {
    std::unordered_map<NodeId, NodeId> remappingTable;
    {
        std::vector<NodeId> originalIds;
        for (const auto& node : m_dataToPaste.m_nodes) {
            originalIds.emplace_back(node->m_id);
        }
        std::vector<NodeId> availableIds = originalIds;
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

    for (auto& node : m_dataToPaste.m_nodes) {
        auto it = remappingTable.find(node->m_id);
        assert((it != remappingTable.end()) && "All node ids should be in the map");
        NodeId newNodeId = it->second;
        node->m_id = newNodeId;

        auto newEnd =
            std::remove_if(node->m_modifiers.begin(), node->m_modifiers.end(),
                           [this, &remappingTable, preserveInConnections, newNodeId,
                            &project](std::unique_ptr<ModifierData>& modData) {
                               if (auto* assignFromData = modData.get()->as<ConnectionModifierData>()) {
                                   NodeId& sourceId = assignFromData->m_sourceId;
                                   auto it = remappingTable.find(sourceId);
                                   if (it == remappingTable.end()) {
                                       if (!preserveInConnections || !project.getNode(sourceId)) {
                                           // Discard it if we're not preserving in-connections OR the source doesn't
                                           // exist.
                                           return true;
                                       }
                                   } else {
                                       // Update the id.
                                       sourceId = it->second;
                                   }
                                   m_connectionsToPaste.emplace_back(ConnectionDescription(newNodeId, *assignFromData));
                                   return true;
                               }
                               return false;
                           });
        node->m_modifiers.erase(newEnd, node->m_modifiers.end());
    }
    return true;
}

void babelwires::PasteNodesCommand::execute(Project& project) const {
    std::unordered_set<UiPosition> occupiedPositions;

    // Find unoccupied UI positions.
    for (const auto& [_, e] : project.getNodes()) {
        occupiedPositions.insert(e->getUiPosition());
    }

    babelwires::UiCoord offset = 0;

    // After 50 attempts, allow duplication locations.
    for (int i = 0; i < 50; ++i) {
        babelwires::UiCoord attemptedOffset = offset;
        for (const auto& elementData : m_dataToPaste.m_nodes) {
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

    for (const auto& elementData : m_dataToPaste.m_nodes) {
        // TODO
        std::unique_ptr<NodeData> clone = elementData->clone();
        clone->m_uiData.m_uiPosition += offset;
        project.addNode(*clone);
    }
    for (const auto& connection : m_connectionsToPaste) {
        auto newModifier = std::make_unique<ConnectionModifierData>();
        newModifier->m_targetPath = connection.m_targetPath;
        newModifier->m_sourceId = connection.m_sourceId;
        newModifier->m_sourcePath = connection.m_sourcePath;
        project.addModifier(connection.m_targetId, *newModifier);
    }
}

void babelwires::PasteNodesCommand::undo(Project& project) const {
    for (const auto& connection : m_connectionsToPaste) {
        project.removeModifier(connection.m_targetId, connection.m_targetPath);
    }
    for (const auto& elementData : m_dataToPaste.m_nodes) {
        project.removeNode(elementData->m_id);
    }
}
