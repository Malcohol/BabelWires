/**
 * The ProjectObserver interprets changes in the project and fires a set of signals useful
 * for the UI.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>

#include <Common/Signal/signal.hpp>

#include <unordered_set>

namespace babelwires {

    class Project;
    class FeatureElement;
    struct UiPosition;
    struct UiSize;
    class Modifier;
    class ConnectionModifier;

    /// This class interpret the changes in the project into a set of signals useful
    /// for the UI.
    ///
    /// Clients will typically want to query the state of the project when
    /// informed about changes. If we fired signals in the middle of processing,
    /// then the project can be in an inconsistent state. Thus, we wait until
    /// all the changes have been performed, and then fire the signals.
    ///
    /// The signals are designed to match the operations a UI will need to perform
    /// to keep itself consistent with the project. Therefore, the signals about
    /// connections describe paths which are truncated to the first collapsed
    /// compound.
    ///
    /// The ignore methods handle the fact that the UI may already have performed
    /// operations, so we don't want it to perform them again. For example, moving
    /// an element in the UI.
    /// TODO: Adjust the UI so non-continuous operations (e.g. add or remove element) do not need this mechanism.
    /// TODO: Think about ways of avoiding it for continuous operations too.
    class ProjectObserver {
      public:
        /// Construct a project observe to observe the given project.
        ProjectObserver(const Project& project);

        /// Fire signals based on the changes observed in the project.
        /// Does not clear the project's changes.
        void interpretChangesAndFireSignals();

        /// Don't fire a signal if this element is observed being added.
        void ignoreAddedElement(ElementId elementId);

        /// Don't fire a signal if this element is observed being removed.
        void ignoreRemovedElement(ElementId elementId);

        /// Don't fire a signal if this element is observed being moved.
        void ignoreMovedElement(ElementId elementId);

        /// Don't fire a signal if this element is observed being resized.
        void ignoreResizedElement(ElementId elementId);

        /// Don't fire a signal if this connection is observed being added.
        void ignoreAddedConnection(ConnectionDescription connection);

        /// Don't fire a signal if this connection is observed being removed.
        void ignoreRemovedConnection(ConnectionDescription connection);

      public:
        /// A feature element was observed being added.
        Signal<const FeatureElement*> m_featureElementWasAdded;

        /// A feature element was observed being removed.
        Signal<ElementId> m_featureElementWasRemoved;

        /// A feature element was observed being moved.
        Signal<ElementId, const UiPosition&> m_featureElementWasMoved;

        /// A feature element was observed being moved.
        Signal<ElementId, const UiSize&> m_featureElementWasResized;

        /// A connection was observed being added.
        Signal<const ConnectionDescription&> m_connectionWasAdded;

        /// A connection was observed being removed.
        Signal<const ConnectionDescription&> m_connectionWasRemoved;

        /// The content of a feature element was observed changing.
        Signal<ElementId> m_contentWasChanged;

      private:
        /// Fire the m_featureElementWasAdded signal, unless the element is to be ignored.
        void featureElementWasAdded(const FeatureElement* featureElement);

        /// Fire the m_featureElementWasRemoved signal, unless the element is to be ignored.
        void featureElementWasRemoved(ElementId elementId);

        /// Fire the m_featureElementWasMoved signal, unless the element is to be ignored.
        void featureElementWasMoved(ElementId elementId, const UiPosition& uiPosition);

        /// Fire the m_featureElementWasResized signal, unless the element is to be ignored.
        void featureElementWasResized(ElementId elementId, const UiSize& newSize);

        /// Fire the m_connectionWasAdded signal, unless the connection is to be ignored.
        void connectionWasAdded(const ConnectionDescription& connection);

        /// Fire the m_connectionWasRemoved signal, unless the connection is to be ignored.
        void connectionWasRemoved(const ConnectionDescription& connection);

        /// Fire the content was changed signal.
        void contentWasChanged(ElementId elementId);

      private:
        /// The project whose changes this object is observing.
        const Project& m_project;

        /// When the flowscene adds a node, we want to ignore it when we observe
        /// changes in the model.
        std::unordered_set<ElementId> m_addedElementsToIgnore;

        /// When the flowscene removes a node, we want to ignore it when we observe
        /// changes in the model.
        std::unordered_set<ElementId> m_removedElementsToIgnore;

        /// When the flowscene moves a node, we want to ignore it when we observe changes
        /// in the model.
        std::unordered_set<ElementId> m_movedElementsToIgnore;

        /// When the flowscene resizes a node, we want to ignore it when we observe changes
        /// in the model.
        std::unordered_set<ElementId> m_resizedElementsToIgnore;

        /// When the flowscene adds a connection, we want to ignore it when we observe
        /// changes in the model.
        std::unordered_set<ConnectionDescription> m_addedConnectionsToIgnore;

        /// When the flowscene removes a connection, we want to ignore it when we observe
        /// changes in the model.
        std::unordered_set<ConnectionDescription> m_removedConnectionsToIgnore;
    };

} // namespace babelwires
