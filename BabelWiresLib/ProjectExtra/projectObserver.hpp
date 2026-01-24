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

#include <BaseLib/Signal/signal.hpp>

#include <unordered_set>

namespace babelwires {

    class Project;
    class Node;
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
    class ProjectObserver {
      public:
        /// Construct a project observe to observe the given project.
        ProjectObserver(const Project& project);

        /// Fire signals based on the changes observed in the project.
        /// Does not clear the project's changes.
        void interpretChangesAndFireSignals();

      public:
        /// A node was observed being added.
        Signal<const Node*> m_nodeWasAdded;

        /// A node was observed being removed.
        Signal<NodeId> m_nodeWasRemoved;

        /// A node was observed being moved.
        Signal<NodeId, const UiPosition&> m_nodeWasMoved;

        /// A node was observed being moved.
        Signal<NodeId, const UiSize&> m_nodeWasResized;

        /// A connection was observed being added.
        Signal<const ConnectionDescription&> m_connectionWasAdded;

        /// A connection was observed being removed.
        Signal<const ConnectionDescription&> m_connectionWasRemoved;

        /// The content of a node was observed changing.
        Signal<NodeId> m_contentWasChanged;

      private:
        /// The project whose changes this object is observing.
        const Project& m_project;
    };

} // namespace babelwires
