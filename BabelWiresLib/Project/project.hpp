/**
 * The Project manages the graph of Nodes, and propagates data from sources to targets.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <map>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    class TargetFileFormatRegistry;
    class SourceFileFormatRegistry;
    class ProcessorFactoryRegistry;
    struct ProjectContext;
    class Modifier;
    struct ModifierData;
    class Node;
    struct NodeData;
    class ConnectionModifier;
    class Path;
    struct UiPosition;
    struct UiSize;

    /// The Project manages the graph of Nodes, and propagates data from sources to targets.
    class Project {
      public:
        /// Construct a new project.
        Project(ProjectContext& context, UserLogger& userLogger);
        virtual ~Project();

        /// Initialize the project as described by the projectData.
        void setProjectData(const ProjectData& projectData);

        /// Get a copy of the data sufficient to reconstruct the state of this project.
        ProjectData extractProjectData() const;

        /// Clear contents and generate a new projectId.
        void clear();

        /// Add a feature element. Will use the ID of the data, if that ID is available.
        /// Returns the ID of the data after it was added.
        NodeId addNode(const NodeData& data);

        /// Remove the element with the given id.
        void removeElement(NodeId id);

        /// Add a modifier as described by the data.
        void addModifier(NodeId elementId, const ModifierData& modifierData);

        /// Remove the modifier at the path from the given element.
        void removeModifier(NodeId elementId, const Path& featurePath);

        /// Add an element to the array at the given path.
        /// Modifiers below the index are adjusted.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        /// This is special-cased to avoid treating all affected modifiers as add/removed,
        /// and possibly triggering arbitrary amount of work.
        // TODO: Much of the work here could be pulled out into the commands. Same with other operations below.
        void addArrayEntries(NodeId elementId, const Path& featurePath, int indexOfNewElement,
                             int numEntriesToAdd, bool ensureModifier);

        /// Remove an element from the array at the given path.
        /// This will adjust the paths in modifiers which are affected.
        /// However, note that this will not remove modifiers.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        void removeArrayEntries(NodeId elementId, const Path& featurePath, int indexOfElementToRemove,
                                int numEntriesToRemove, bool ensureModifier);

        /// Adjust modifiers and connections which point into an array to adapt to shifted array elements.
        /// If adjustment is positive, then modifiers at and above startIndex are modified.
        /// If adjustment is negative, then modifiers at (startIndex - adjustment) and above are modified.
        void adjustModifiersInArrayElements(NodeId elementId, const Path& pathToArray, ArrayIndex startIndex,
                                            int adjustment);

        /// Activate an optional in a RecordType.
        void activateOptional(NodeId elementId, const Path& pathToRecord, ShortId optional,
                              bool ensureModifier);

        /// Deactivate an optional in a RecordType.
        /// Note that this method is not responsible for removing modifiers.
        void deactivateOptional(NodeId elementId, const Path& pathToRecord, ShortId optional,
                                bool ensureModifier);

        /// Set the Ui position of the element.
        void setNodePosition(NodeId elementId, const UiPosition& newPosition);

        /// Sets the Ui size of the element's contents.
        void setNodeContentsSize(NodeId elementId, const UiSize& newSize);

        const TargetFileFormatRegistry& getFactoryFormatRegistry() const;
        const SourceFileFormatRegistry& getFileFormatRegistry() const;

        Node* getNode(NodeId id);
        const Node* getNode(NodeId id) const;

        /// Reload the source file.
        /// File exceptions are caught and written to the userLogger.
        void tryToReloadSource(NodeId id);

        /// Save the target file non-interactively.
        /// File exceptions are caught and written to the userLogger.
        void tryToSaveTarget(NodeId id);

        /// Reload all the source file.
        /// File exceptions are caught and written to the userLogger.
        void tryToReloadAllSources();

        /// Save all the target files non-interactively.
        /// File exceptions are caught and written to the userLogger.
        void tryToSaveAllTargets();

        ///
        const std::map<NodeId, std::unique_ptr<Node>>& getNodes() const;

        /// Process any changes in the whole project.
        void process();

        /// Mark all features in the project as unchanged.
        void clearChanges();

        /// Information about the connections between elements.
        /// The connectionInfo includes elements and modifiers which failed.
        struct ConnectionInfo {
            using Connections = std::vector<std::tuple<ConnectionModifier*, Node*>>;
            using ConnectionMap = std::unordered_map<const Node*, Connections>;

            /// Information about the elements a given element depends on.
            /// There is an entry for an element only if it has an incoming connection.
            ConnectionMap m_dependsOn;

            /// Information about the elements which depend on a given element.
            /// There is an entry for an element only if it has an outgoing connection.
            ConnectionMap m_requiredFor;

            /// Connections that are missing their source, paired with their owner.
            Connections m_brokenConnections;
        };

        /// Get information about the connections between elements.
        const ConnectionInfo& getConnectionInfo() const;

        /// Get the feature elements which have been removed since the last time
        /// changes were cleared.
        const std::map<NodeId, std::unique_ptr<Node>>& getRemovedElements() const;

        /// Get the ProjectId of the current project.
        ProjectId getProjectId() const;

        /// If any of the ids are unavailable, replace them by currently available ones.
        void updateWithAvailableIds(std::vector<NodeId>& idsInOut) const;

      private:
        /// Return an available id, using the provided hint if it is valid and available.
        NodeId reserveNodeId(NodeId hint);

        /// Mark the connection cache as invalid, so the next time it is queried, it gets recomputed.
        void setConnectionCacheInvalid();

        /// If the output of e has any changes, propagate them to the input features of connected
        /// elements, as described by the requiredForMap.
        void propagateChanges(const Node* e);

        /// Set the ProjectId to a random value.
        void randomizeProjectId();

        void addConnectionToCache(Node* element, ConnectionModifier* data);

        void removeConnectionFromCache(Node* element, ConnectionModifier* data);

        void validateConnectionCache() const;

        Node* addNodeWithoutCachingConnection(const NodeData& data);
        void addNodeConnectionsToCache(Node* element);

      private:
        ProjectContext& m_context;

        /// The userLogger is only used for logging user-visible errors, warnings and messages.
        /// It is not intended for debug logging.
        UserLogger& m_userLogger;

        /// The ID of the current project. Randomly assigned on construction and clear.
        ProjectId m_projectId;

        /// 0 == INVALID_ELEMENT_ID and is never used for an actual element.
        NodeId m_maxAssignedNodeId = 0;

        /// A map of elements, keyed by ElementID.
        std::map<NodeId, std::unique_ptr<Node>> m_nodes;

        /// Cache of connection information.
        ConnectionInfo m_connectionCache;

        /// Feature elements which have been removed since the last time changes were cleared.
        /// Use a map because we iterate.
        std::map<NodeId, std::unique_ptr<Node>> m_removedNodes;
    };

} // namespace babelwires
