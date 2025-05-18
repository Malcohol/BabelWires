/**
 * The ProjectObserver interprets changes in the project and fires a set of signals useful
 * for the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/projectObserver.hpp>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::ProjectObserver::ProjectObserver(const Project& project)
    : m_project(project) {}

namespace {
    using State = babelwires::EditTree::State;

    void addToConnections(std::unordered_set<babelwires::ConnectionDescription>& connections, State state,
                          babelwires::ConnectionDescription&& connectionDesc,
                          const babelwires::Node* sourceElement,
                          const babelwires::Node* targetElement) {
        sourceElement->getEdits().truncatePathAtFirstCollapsedNode(connectionDesc.m_sourcePath, state);
        targetElement->getEdits().truncatePathAtFirstCollapsedNode(connectionDesc.m_targetPath, state);
        connections.emplace(std::move(connectionDesc));
    }

    void addToConnections(std::unordered_set<babelwires::ConnectionDescription>& connections, State state,
                          babelwires::ConnectionDescription&& connectionDesc, const babelwires::Project& project,
                          const babelwires::Node* targetElement) {
        const babelwires::Node* sourceElement = project.getNode(connectionDesc.m_sourceId);
        if (!sourceElement) {
            const auto it = project.getRemovedNodes().find(connectionDesc.m_sourceId);
            assert((it != project.getRemovedNodes().end()) && "Expecting to find the source of a live connection.");
            sourceElement = it->second.get();
        }
        assert(sourceElement && "Expecting to find the source of a live connection.");
        addToConnections(connections, state, std::move(connectionDesc), sourceElement, targetElement);
    }

    void addAllLiveInConnections(const babelwires::Project::ConnectionInfo& connectionInfo,
                                 const babelwires::Node* targetElement,
                                 std::unordered_set<babelwires::ConnectionDescription>& connections, State state) {
        const babelwires::NodeId nodeId = targetElement->getNodeId();
        const auto& inConnections = connectionInfo.m_dependsOn.find(targetElement);
        if (inConnections != connectionInfo.m_dependsOn.end()) {
            for (const auto& connection : inConnections->second) {
                const babelwires::ConnectionModifier& connectionModifier = *std::get<0>(connection);
                if (connectionModifier.isConnected() &&
                    ((state == State::CurrentState) ||
                     !connectionModifier.isChanged(babelwires::Modifier::Changes::ModifierIsNew |
                                                   babelwires::Modifier::Changes::ModifierMoved |
                                                   babelwires::Modifier::Changes::ModifierConnected))) {
                    const babelwires::Node* sourceElement = std::get<1>(connection);
                    addToConnections(connections, state,
                                     babelwires::ConnectionDescription(nodeId, connectionModifier.getModifierData()),
                                     sourceElement, targetElement);
                }
            }
        }
    }

    void addAllLiveOutConnections(const babelwires::Project::ConnectionInfo& connectionInfo,
                                  const babelwires::Node* sourceElement,
                                  std::unordered_set<babelwires::ConnectionDescription>& connections, State state) {
        const auto& outConnections = connectionInfo.m_requiredFor.find(sourceElement);
        if (outConnections != connectionInfo.m_requiredFor.end()) {
            for (auto&& connection : outConnections->second) {
                const babelwires::Node* const targetElement = std::get<1>(connection);
                const babelwires::ConnectionModifier& connectionModifier = *std::get<0>(connection);
                if (connectionModifier.isConnected() &&
                    ((state == State::CurrentState) ||
                     !connectionModifier.isChanged(babelwires::Modifier::Changes::ModifierIsNew |
                                                   babelwires::Modifier::Changes::ModifierMoved |
                                                   babelwires::Modifier::Changes::ModifierConnected))) {
                    addToConnections(connections, state,
                                     babelwires::ConnectionDescription(targetElement->getNodeId(),
                                                                       connectionModifier.getModifierData()),
                                     sourceElement, targetElement);
                }
            }
        }
    }
} // namespace

void babelwires::ProjectObserver::interpretChangesAndFireSignals() {
    // NOTE: We assume that we don't see the same node or connection
    // added and removed with changes being processed in between.

    // We don't give special handling here to the modifiers when a Node
    // fails or recovers as a whole. The modifiers of such a node should fail
    // or recover individually, as appropriate.

    // Just those elements with changes.
    std::vector<const Node*> nodesWithChanges;
    nodesWithChanges.reserve(m_project.getNodes().size());

    // const Node::Changes someStructureChange = Node::Changes::FeatureStructureChanged |
    // Node::Changes::CompoundExpandedOrCollapsed;

    for (const auto& pair : m_project.getNodes()) {
        const Node* const node = pair.second.get();
        const NodeId nodeId = node->getNodeId();

        if (node->isChanged(Node::Changes::SomethingChanged)) {
            nodesWithChanges.emplace_back(node);
        }
    }

    const Project::ConnectionInfo& connectionInfo = m_project.getConnectionInfo();

    // We compile the changes into these containers, and apply them below.
    // Note: The nodeeditor based UI does not know how to handle nodes which have changed
    // their number of ports, so we have to remove and re-add such nodes.
    // TODO: Consider using a set rather than an unordered_set, since the iteration below
    // will introduce non-determinacy.
    std::vector<const Node*> nodesToCreate;
    std::unordered_set<ConnectionDescription> connectionsToAdd;
    std::unordered_set<ConnectionDescription> connectionsToRemove;
    std::vector<const Node*> nodesToRemove;

    auto allModifiersWereRemoved = [&connectionsToRemove, this](const Node* targetElement, NodeId targetId,
                                                                const auto& modifiers) {
        for (const auto* modifier : modifiers) {
            if (const auto* connectionModifier = modifier->asConnectionModifier()) {
                // Exclude new connections since the UI won't have have seen them yet.
                // TODO A change which caused a structural change AND caused a connection to recover might need extra treatment here.
                // Possibly I need to exclude recovered connections too, but I'm not sure recovery is specific enough.
                if (connectionModifier->isConnected() && !connectionModifier->isChanged(Modifier::Changes::ModifierIsNew)) {
                    addToConnections(connectionsToRemove, State::PreviousState,
                                     ConnectionDescription(targetId, connectionModifier->getModifierData()), m_project,
                                     targetElement);
                }
            }
        }
    };

    for (const auto& [_, node] : m_project.getRemovedNodes()) {
        assert(!node->isChanged(Node::Changes::NodeIsNew) &&
               "Changes should have been processed between new elements being removed.");
        const NodeId nodeId = node->getNodeId();
        nodesToRemove.emplace_back(node.get());
        allModifiersWereRemoved(node.get(), nodeId, node->getEdits().modifierRange());
        allModifiersWereRemoved(node.get(), nodeId, node->getRemovedModifiers());
    }

    for (const auto* node : nodesWithChanges) {
        const NodeId nodeId = node->getNodeId();

        if (node->isChanged(Node::Changes::NodeIsNew)) {
            nodesToCreate.emplace_back(node);
            addAllLiveInConnections(connectionInfo, node, connectionsToAdd, State::CurrentState);
        } else {
            allModifiersWereRemoved(node, nodeId, node->getRemovedModifiers());
            if (node->isChanged(Node::Changes::ModifierDisconnected)) {
                for (const auto& connectionModifier :
                     node->getEdits().modifierRange<babelwires::ConnectionModifier>()) {
                    if (connectionModifier->isChanged(Modifier::Changes::ModifierDisconnected) &&
                        !connectionModifier->isChanged(Modifier::Changes::ModifierIsNew)) {
                        addToConnections(connectionsToRemove, State::PreviousState,
                                         ConnectionDescription(nodeId, connectionModifier->getModifierData()),
                                         m_project, node);
                    }
                }
            }

            if (node->isChanged(Node::Changes::UiPositionChanged)) {
                m_nodeWasMoved.fire(nodeId, node->getUiPosition());
            }

            if (node->isChanged(Node::Changes::UiSizeChanged)) {
                m_nodeWasResized.fire(nodeId, node->getUiSize());
            }

            // const bool hasStructureChange = node->isChanged(someStructureChange);
            const bool hasStructureChange =
                node->getContentsCache().isChanged(ContentsCache::Changes::StructureChanged);
            if (hasStructureChange) {
                addAllLiveInConnections(connectionInfo, node, connectionsToRemove, State::PreviousState);
                addAllLiveOutConnections(connectionInfo, node, connectionsToRemove, State::PreviousState);
                nodesToRemove.emplace_back(node);
                nodesToCreate.emplace_back(node);
                addAllLiveInConnections(connectionInfo, node, connectionsToAdd, State::CurrentState);
                addAllLiveOutConnections(connectionInfo, node, connectionsToAdd, State::CurrentState);
            }

            if (node->isChanged(Node::Changes::ModifierAdded |
                                          Node::Changes::ModifierConnected)) {
                for (const auto& connectionModifier :
                     node->getEdits().modifierRange<babelwires::ConnectionModifier>()) {
                    if (connectionModifier->isChanged(Modifier::Changes::ModifierIsNew |
                                                      Modifier::Changes::ModifierConnected) &&
                        !connectionModifier->isChanged(Modifier::Changes::ModifierDisconnected)) {
                        ConnectionDescription connection(nodeId, connectionModifier->getModifierData());
                        if (!hasStructureChange) {
                            // Normal case: Just remember to add the connection.
                            addToConnections(connectionsToAdd, State::CurrentState, std::move(connection), m_project,
                                             node);

                        } 
                        //else if (m_addedConnectionsToIgnore.erase(connection)) {
                        //    // Stop the connection from being skipped when the nodesToCreate are processed.
                        //    // This might do nothing if the node was not going to be ignored.
                        //    addToConnections(connectionsToRemove, State::PreviousState, std::move(connection),
                        //                     m_project, node);
                        //}
                    }
                }
            }
        }
    }

    // Now fire the callbacks in the appropriate order.

    for (const auto& connection : connectionsToRemove) {
        m_connectionWasRemoved.fire(connection);
    }

    for (const auto& node : nodesToRemove) {
        m_nodeWasRemoved.fire(node->getNodeId());
    }

    for (auto&& node : nodesToCreate) {
        m_nodeWasAdded.fire(node);
    }

    for (auto&& connection : connectionsToAdd) {
        m_connectionWasAdded.fire(connection);
    }

    // Update the nodes that didn't have structural changes.
    for (const auto* node : nodesWithChanges) {
        if (node->isChanged(
                Node::Changes::FeatureChangesMask | Node::Changes::NodeLabelChanged |
                Node::Changes::ModifierAdded | Node::Changes::ModifierRemoved |
                Node::Changes::FileChanged | Node::Changes::NodeFailed |
                Node::Changes::NodeRecovered) &&
            !node->getContentsCache().isChanged(ContentsCache::Changes::StructureChanged) &&
            !node->isChanged(babelwires::Node::Changes::NodeIsNew)) {
            m_contentWasChanged.fire(node->getNodeId());
        }
    }
}
