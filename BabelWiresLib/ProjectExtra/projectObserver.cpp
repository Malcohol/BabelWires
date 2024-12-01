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

void babelwires::ProjectObserver::featureElementWasAdded(const Node* featureElement) {
    const ElementId elementId = featureElement->getElementId();
    auto ignoreIt = m_addedElementsToIgnore.find(elementId);
    if (ignoreIt == m_addedElementsToIgnore.end()) {
        m_featureElementWasAdded.fire(featureElement);
    } else {
        m_addedElementsToIgnore.erase(elementId);
    }
}

void babelwires::ProjectObserver::featureElementWasRemoved(ElementId elementId) {
    auto ignoreIt = m_removedElementsToIgnore.find(elementId);
    if (ignoreIt == m_removedElementsToIgnore.end()) {
        m_featureElementWasRemoved.fire(elementId);
    } else {
        m_removedElementsToIgnore.erase(ignoreIt);
    }
}

void babelwires::ProjectObserver::featureElementWasMoved(ElementId elementId, const UiPosition& uiPosition) {
    auto ignoreIt = m_movedElementsToIgnore.find(elementId);
    if (ignoreIt == m_movedElementsToIgnore.end()) {
        m_featureElementWasMoved.fire(elementId, uiPosition);
    } else {
        m_movedElementsToIgnore.erase(ignoreIt);
    }
}

void babelwires::ProjectObserver::featureElementWasResized(ElementId elementId, const UiSize& newSize) {
    auto ignoreIt = m_resizedElementsToIgnore.find(elementId);
    if (ignoreIt == m_resizedElementsToIgnore.end()) {
        m_featureElementWasResized.fire(elementId, newSize);
    } else {
        m_resizedElementsToIgnore.erase(ignoreIt);
    }
}

void babelwires::ProjectObserver::connectionWasAdded(const ConnectionDescription& connection) {
    auto ignoreIt = m_addedConnectionsToIgnore.find(connection);
    if (ignoreIt == m_addedConnectionsToIgnore.end()) {
        m_connectionWasAdded.fire(connection);
    } else {
        m_addedConnectionsToIgnore.erase(ignoreIt);
    }
}

void babelwires::ProjectObserver::connectionWasRemoved(const ConnectionDescription& connection) {
    auto ignoreIt = m_removedConnectionsToIgnore.find(connection);
    if (ignoreIt == m_removedConnectionsToIgnore.end()) {
        m_connectionWasRemoved.fire(connection);
    } else {
        m_removedConnectionsToIgnore.erase(ignoreIt);
    }
}

void babelwires::ProjectObserver::contentWasChanged(ElementId elementId) {
    m_contentWasChanged.fire(elementId);
}

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
            const auto it = project.getRemovedElements().find(connectionDesc.m_sourceId);
            assert((it != project.getRemovedElements().end()) && "Expecting to find the source of a live connection.");
            sourceElement = it->second.get();
        }
        assert(sourceElement && "Expecting to find the source of a live connection.");
        addToConnections(connections, state, std::move(connectionDesc), sourceElement, targetElement);
    }

    void addAllLiveInConnections(const babelwires::Project::ConnectionInfo& connectionInfo,
                                 const babelwires::Node* targetElement,
                                 std::unordered_set<babelwires::ConnectionDescription>& connections, State state) {
        const babelwires::ElementId elementId = targetElement->getElementId();
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
                                     babelwires::ConnectionDescription(elementId, connectionModifier.getModifierData()),
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
                                     babelwires::ConnectionDescription(targetElement->getElementId(),
                                                                       connectionModifier.getModifierData()),
                                     sourceElement, targetElement);
                }
            }
        }
    }
} // namespace

void babelwires::ProjectObserver::interpretChangesAndFireSignals() {
    // NOTE: We assume that we don't see the same element or connection
    // added and removed with changes being processed in between.

    // We don't give special handling here to the modifiers when a feature element
    // fails or recovers as a whole. The modifiers of such an element should fail
    // or recover individually, as appropriate.

    // Just those elements with changes.
    std::vector<const Node*> featureElementsWithChanges;
    featureElementsWithChanges.reserve(m_project.getElements().size());

    // const Node::Changes someStructureChange = Node::Changes::FeatureStructureChanged |
    // Node::Changes::CompoundExpandedOrCollapsed;

    for (const auto& pair : m_project.getElements()) {
        const Node* const featureElement = pair.second.get();
        const ElementId elementId = featureElement->getElementId();

        if (featureElement->isChanged(Node::Changes::SomethingChanged)) {
            featureElementsWithChanges.emplace_back(featureElement);
        }
    }

    const Project::ConnectionInfo& connectionInfo = m_project.getConnectionInfo();

    // We compile the changes into these containers, and apply them below.
    // Note: The nodeeditor based UI does not know how to handle nodes which have changed
    // their number of ports, so we have to remove and re-add such nodes.
    // TODO: Consider using a set rather than an unordered_set, since the iteratation below
    // will introduce non-determinacy.
    std::vector<const Node*> nodesToCreate;
    std::unordered_set<ConnectionDescription> connectionsToAdd;
    std::unordered_set<ConnectionDescription> connectionsToRemove;
    std::vector<const Node*> nodesToRemove;

    auto allModifiersWereRemoved = [&connectionsToRemove, this](const Node* targetElement, ElementId targetId,
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

    for (const auto& [_, featureElement] : m_project.getRemovedElements()) {
        assert(!featureElement->isChanged(Node::Changes::NodeIsNew) &&
               "Changes should have been processed between new elements being removed.");
        const ElementId elementId = featureElement->getElementId();
        nodesToRemove.emplace_back(featureElement.get());
        allModifiersWereRemoved(featureElement.get(), elementId, featureElement->getEdits().modifierRange());
        allModifiersWereRemoved(featureElement.get(), elementId, featureElement->getRemovedModifiers());
    }

    for (const auto* featureElement : featureElementsWithChanges) {
        const ElementId elementId = featureElement->getElementId();

        if (featureElement->isChanged(Node::Changes::NodeIsNew)) {
            nodesToCreate.emplace_back(featureElement);
            addAllLiveInConnections(connectionInfo, featureElement, connectionsToAdd, State::CurrentState);
        } else {
            allModifiersWereRemoved(featureElement, elementId, featureElement->getRemovedModifiers());
            if (featureElement->isChanged(Node::Changes::ModifierDisconnected)) {
                for (const auto& connectionModifier :
                     featureElement->getEdits().modifierRange<babelwires::ConnectionModifier>()) {
                    if (connectionModifier->isChanged(Modifier::Changes::ModifierDisconnected) &&
                        !connectionModifier->isChanged(Modifier::Changes::ModifierIsNew)) {
                        addToConnections(connectionsToRemove, State::PreviousState,
                                         ConnectionDescription(elementId, connectionModifier->getModifierData()),
                                         m_project, featureElement);
                    }
                }
            }

            if (featureElement->isChanged(Node::Changes::UiPositionChanged)) {
                featureElementWasMoved(elementId, featureElement->getUiPosition());
            }

            if (featureElement->isChanged(Node::Changes::UiSizeChanged)) {
                featureElementWasResized(elementId, featureElement->getUiSize());
            }

            // const bool hasStructureChange = featureElement->isChanged(someStructureChange);
            const bool hasStructureChange =
                featureElement->getContentsCache().isChanged(ContentsCache::Changes::StructureChanged);
            if (hasStructureChange) {
                addAllLiveInConnections(connectionInfo, featureElement, connectionsToRemove, State::PreviousState);
                addAllLiveOutConnections(connectionInfo, featureElement, connectionsToRemove, State::PreviousState);
                nodesToRemove.emplace_back(featureElement);
                nodesToCreate.emplace_back(featureElement);
                addAllLiveInConnections(connectionInfo, featureElement, connectionsToAdd, State::CurrentState);
                addAllLiveOutConnections(connectionInfo, featureElement, connectionsToAdd, State::CurrentState);
            }

            if (featureElement->isChanged(Node::Changes::ModifierAdded |
                                          Node::Changes::ModifierConnected)) {
                for (const auto& connectionModifier :
                     featureElement->getEdits().modifierRange<babelwires::ConnectionModifier>()) {
                    if (connectionModifier->isChanged(Modifier::Changes::ModifierIsNew |
                                                      Modifier::Changes::ModifierConnected) &&
                        !connectionModifier->isChanged(Modifier::Changes::ModifierDisconnected)) {
                        ConnectionDescription connection(elementId, connectionModifier->getModifierData());
                        if (!hasStructureChange) {
                            // Normal case: Just remember to add the connection.
                            addToConnections(connectionsToAdd, State::CurrentState, std::move(connection), m_project,
                                             featureElement);

                        } else if (m_addedConnectionsToIgnore.erase(connection)) {
                            // Stop the connection from being skipped when the nodesToCreate are processed.
                            // This might do nothing if the element was not going to be ignored.
                            addToConnections(connectionsToRemove, State::PreviousState, std::move(connection),
                                             m_project, featureElement);
                        }
                    }
                }
            }
        }
    }

    // Now fire the callbacks in the appropriate order.

    for (const auto& connection : connectionsToRemove) {
        connectionWasRemoved(connection);
    }

    for (const auto& featureElement : nodesToRemove) {
        featureElementWasRemoved(featureElement->getElementId());
    }

    for (auto&& featureElement : nodesToCreate) {
        featureElementWasAdded(featureElement);
    }

    for (auto&& connection : connectionsToAdd) {
        connectionWasAdded(connection);
    }

    // Update the nodes that didn't have structural changes.
    for (const auto* featureElement : featureElementsWithChanges) {
        if (featureElement->isChanged(
                Node::Changes::FeatureChangesMask | Node::Changes::NodeLabelChanged |
                Node::Changes::ModifierAdded | Node::Changes::ModifierRemoved |
                Node::Changes::FileChanged | Node::Changes::NodeFailed |
                Node::Changes::NodeRecovered) &&
            !featureElement->getContentsCache().isChanged(ContentsCache::Changes::StructureChanged) &&
            !featureElement->isChanged(babelwires::Node::Changes::NodeIsNew)) {
            contentWasChanged(featureElement->getElementId());
        }
    }

    assert((m_addedElementsToIgnore.empty()) && "Did not observe the newly created nodes.");
    assert(m_removedElementsToIgnore.empty() && "Did not observe the newly deleted nodes.");
    assert((m_addedConnectionsToIgnore.empty()) && "Did not observe the newly added connections");
    assert(m_removedConnectionsToIgnore.empty() && "Did not observe all newly deleted connections");
    assert((m_movedElementsToIgnore.empty()) && "Did not observe the newly moved nodes.");
    assert((m_resizedElementsToIgnore.empty()) && "Did not observe the newly resized nodes.");
}

void babelwires::ProjectObserver::ignoreAddedElement(ElementId elementId) {
    m_addedElementsToIgnore.insert(elementId);
}

void babelwires::ProjectObserver::ignoreRemovedElement(ElementId elementId) {
    m_removedElementsToIgnore.insert(elementId);
}

void babelwires::ProjectObserver::ignoreAddedConnection(ConnectionDescription connection) {
    m_addedConnectionsToIgnore.insert(std::move(connection));
}

void babelwires::ProjectObserver::ignoreRemovedConnection(ConnectionDescription connection) {
    m_removedConnectionsToIgnore.insert(std::move(connection));
}

void babelwires::ProjectObserver::ignoreMovedElement(ElementId elementId) {
    m_movedElementsToIgnore.insert(elementId);
}

void babelwires::ProjectObserver::ignoreResizedElement(ElementId elementId) {
    m_resizedElementsToIgnore.insert(elementId);
}
