/**
 * The ProjectObserver interprets changes in the project and fires a set of signals useful
 * for the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/ProjectExtra/projectObserver.hpp"

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include <cassert>

babelwires::ProjectObserver::ProjectObserver(const Project& project)
    : m_project(project) {}

void babelwires::ProjectObserver::featureElementWasAdded(const FeatureElement* featureElement) {
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
                          const babelwires::FeatureElement* sourceElement,
                          const babelwires::FeatureElement* targetElement) {
        sourceElement->getEdits().truncatePathAtFirstCollapsedNode(connectionDesc.m_pathToSourceFeature, state);
        targetElement->getEdits().truncatePathAtFirstCollapsedNode(connectionDesc.m_pathToTargetFeature, state);
        connections.emplace(std::move(connectionDesc));
    }

    void addToConnections(std::unordered_set<babelwires::ConnectionDescription>& connections, State state,
                          babelwires::ConnectionDescription&& connectionDesc, const babelwires::Project& project,
                          const babelwires::FeatureElement* targetElement) {
        const babelwires::FeatureElement* sourceElement = project.getFeatureElement(connectionDesc.m_sourceId);
        if (!sourceElement) {
            const auto it = project.getRemovedElements().find(connectionDesc.m_sourceId);
            assert((it != project.getRemovedElements().end()) && "Expecting to find the source of a live connection.");
            sourceElement = it->second.get();
        }
        assert(sourceElement && "Expecting to find the source of a live connection.");
        addToConnections(connections, state, std::move(connectionDesc), sourceElement, targetElement);
    }

    void addAllLiveInConnections(const babelwires::Project::ConnectionInfo& connectionInfo,
                                 const babelwires::FeatureElement* targetElement,
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
                    const babelwires::FeatureElement* sourceElement = std::get<1>(connection);
                    addToConnections(connections, state,
                                     babelwires::ConnectionDescription(elementId, connectionModifier.getModifierData()),
                                     sourceElement, targetElement);
                }
            }
        }
    }

    void addAllLiveOutConnections(const babelwires::Project::ConnectionInfo& connectionInfo,
                                  const babelwires::FeatureElement* sourceElement,
                                  std::unordered_set<babelwires::ConnectionDescription>& connections, State state) {
        const auto& outConnections = connectionInfo.m_requiredFor.find(sourceElement);
        if (outConnections != connectionInfo.m_requiredFor.end()) {
            for (auto&& connection : outConnections->second) {
                const babelwires::FeatureElement* const targetElement = std::get<1>(connection);
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
    std::vector<const FeatureElement*> featureElementsWithChanges;
    featureElementsWithChanges.reserve(m_project.getElements().size());

    // const FeatureElement::Changes someStructureChange = FeatureElement::Changes::FeatureStructureChanged |
    // FeatureElement::Changes::CompoundExpandedOrCollapsed;

    for (const auto& pair : m_project.getElements()) {
        const FeatureElement* const featureElement = pair.second.get();
        const ElementId elementId = featureElement->getElementId();

        if (featureElement->isChanged(FeatureElement::Changes::SomethingChanged)) {
            featureElementsWithChanges.emplace_back(featureElement);
        }
    }

    const Project::ConnectionInfo& connectionInfo = m_project.getConnectionInfo();

    // We compile the changes into these containers, and apply them below.
    // Note: The nodeeditor based UI does not know how to handle nodes which have changed
    // their number of ports, so we have to remove and re-add such nodes.
    // TODO: Consider using a set rather than an unordered_set, since the iteratation below
    // will introduce non-determinacy.
    std::vector<const FeatureElement*> nodesToCreate;
    std::unordered_set<ConnectionDescription> connectionsToAdd;
    std::unordered_set<ConnectionDescription> connectionsToRemove;
    std::vector<const FeatureElement*> nodesToRemove;

    auto allModifiersWereRemoved = [&connectionsToRemove, this](const FeatureElement* targetElement, ElementId targetId,
                                                                const auto& modifiers) {
        for (const auto* modifier : modifiers) {
            if (const auto* connectionModifier = modifier->asConnectionModifier()) {
                if (connectionModifier->isConnected()) {
                    addToConnections(connectionsToRemove, State::PreviousState,
                                     ConnectionDescription(targetId, connectionModifier->getModifierData()), m_project,
                                     targetElement);
                }
            }
        }
    };

    for (const auto& [_, featureElement] : m_project.getRemovedElements()) {
        assert(!featureElement->isChanged(FeatureElement::Changes::FeatureElementIsNew) &&
               "Changes should have been processed between new elements being removed.");
        const ElementId elementId = featureElement->getElementId();
        nodesToRemove.emplace_back(featureElement.get());
        allModifiersWereRemoved(featureElement.get(), elementId, featureElement->getEdits().modifierRange());
        allModifiersWereRemoved(featureElement.get(), elementId, featureElement->getRemovedModifiers());
    }

    for (const auto* featureElement : featureElementsWithChanges) {
        const ElementId elementId = featureElement->getElementId();

        if (featureElement->isChanged(FeatureElement::Changes::FeatureElementIsNew)) {
            nodesToCreate.emplace_back(featureElement);
            addAllLiveInConnections(connectionInfo, featureElement, connectionsToAdd, State::CurrentState);
        } else {
            allModifiersWereRemoved(featureElement, elementId, featureElement->getRemovedModifiers());
            if (featureElement->isChanged(FeatureElement::Changes::ModifierDisconnected)) {
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

            if (featureElement->isChanged(FeatureElement::Changes::UiPositionChanged)) {
                featureElementWasMoved(elementId, featureElement->getUiPosition());
            }

            if (featureElement->isChanged(FeatureElement::Changes::UiSizeChanged)) {
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

            if (featureElement->isChanged(FeatureElement::Changes::ModifierAdded |
                                          FeatureElement::Changes::ModifierConnected)) {
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
                FeatureElement::Changes::FeatureChangesMask | FeatureElement::Changes::FeatureElementLabelChanged |
                FeatureElement::Changes::ModifierAdded | FeatureElement::Changes::ModifierRemoved |
                FeatureElement::Changes::FileChanged | FeatureElement::Changes::FeatureElementFailed |
                FeatureElement::Changes::FeatureElementRecovered) &&
            !featureElement->getContentsCache().isChanged(ContentsCache::Changes::StructureChanged) &&
            !featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementIsNew)) {
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
