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

        /// Add a Node. Will use the ID of the data, if that ID is available.
        /// Returns the ID of the data after it was added.
        NodeId addNode(const NodeData& data);

        /// Remove the Node with the given id.
        void removeNode(NodeId id);

        /// Add a modifier as described by the data.
        /// Adding a modifier applies its effect, unless applyModifier is false.
        void addModifier(NodeId nodeId, const ModifierData& modifierData, bool applyModifier = true);

        /// Remove the modifier at the path from the given Node.
        /// Removing a modifier undoes its effect, unless unapplyModifier is false.
        void removeModifier(NodeId nodeId, const Path& featurePath, bool unapplyModifier = true);

        /// Add an element to the array at the given path.
        /// Modifiers below the index are adjusted.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        /// This is special-cased to avoid treating all affected modifiers as add/removed,
        /// and possibly triggering arbitrary amount of work.
        // TODO: Much of the work here could be pulled out into the commands. Same with other operations below.
        void addArrayEntries(NodeId nodeId, const Path& featurePath, int indexOfNewElement,
                             int numEntriesToAdd, bool ensureModifier);

        /// Remove an element from the array at the given path.
        /// This will adjust the paths in modifiers which are affected.
        /// However, note that this will not remove modifiers.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        void removeArrayEntries(NodeId nodeId, const Path& featurePath, int indexOfElementToRemove,
                                int numEntriesToRemove, bool ensureModifier);

        /// Adjust modifiers and connections which point into an array to adapt to shifted array elements.
        /// If adjustment is positive, then modifiers at and above startIndex are modified.
        /// If adjustment is negative, then modifiers at (startIndex - adjustment) and above are modified.
        void adjustModifiersInArrayElements(NodeId nodeId, const Path& pathToArray, ArrayIndex startIndex,
                                            int adjustment);

        /// Activate an optional in a RecordType.
        void activateOptional(NodeId nodeId, const Path& pathToRecord, ShortId optional,
                              bool ensureModifier);

        /// Deactivate an optional in a RecordType.
        /// Note that this method is not responsible for removing modifiers.
        void deactivateOptional(NodeId nodeId, const Path& pathToRecord, ShortId optional,
                                bool ensureModifier);

        /// Set the Ui position of the Node.
        void setNodePosition(NodeId nodeId, const UiPosition& newPosition);

        /// Sets the Ui size of the Node's contents.
        void setNodeContentsSize(NodeId nodeId, const UiSize& newSize);

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

        /// Information about the connections between Nodes.
        /// The connectionInfo includes Nodes and modifiers which failed.
        struct ConnectionInfo {
            using Connections = std::vector<std::tuple<ConnectionModifier*, Node*>>;
            using ConnectionMap = std::unordered_map<const Node*, Connections>;

            /// Information about the Nodes a given Node depends on.
            /// There is an entry for a Node only if it has an incoming connection.
            ConnectionMap m_dependsOn;

            /// Information about the Nodes which depend on a given Node.
            /// There is an entry for a Node only if it has an outgoing connection.
            ConnectionMap m_requiredFor;

            /// Connections that are missing their source, paired with their owner.
            Connections m_brokenConnections;
        };

        /// Get information about the connections between Nodes.
        const ConnectionInfo& getConnectionInfo() const;

        /// Get the Nodes which have been removed since the last time
        /// changes were cleared.
        const std::map<NodeId, std::unique_ptr<Node>>& getRemovedNodes() const;

        /// Get the ProjectId of the current project.
        ProjectId getProjectId() const;

        /// If any of the ids are unavailable, replace them by currently available ones.
        void updateWithAvailableIds(std::vector<NodeId>& idsInOut) const;

        /// Return an available id, using the provided hint if it is valid and available.
        NodeId reserveNodeId(NodeId hint = INVALID_NODE_ID);

      private:
        /// Mark the connection cache as invalid, so the next time it is queried, it gets recomputed.
        void setConnectionCacheInvalid();

        /// If the output of e has any changes, propagate them to the input features of connected
        /// Nodes, as described by the requiredForMap.
        void propagateChanges(const Node* e);

        /// Set the ProjectId to a random value.
        void randomizeProjectId();

        void addConnectionToCache(Node* node, ConnectionModifier* data);

        void removeConnectionFromCache(Node* node, ConnectionModifier* data);

        void validateConnectionCache() const;

        Node* addNodeWithoutCachingConnection(const NodeData& data);
        void addNodeConnectionsToCache(Node* node);

      private:
        ProjectContext& m_context;

        /// The userLogger is only used for logging user-visible errors, warnings and messages.
        /// It is not intended for debug logging.
        UserLogger& m_userLogger;

        /// The ID of the current project. Randomly assigned on construction and clear.
        ProjectId m_projectId;

        /// 0 == INVALID_NODE_ID and is never used for an actual Node.
        NodeId m_maxAssignedNodeId = 0;

        /// A map of Nodes, keyed by NodeId.
        std::map<NodeId, std::unique_ptr<Node>> m_nodes;

        /// Cache of connection information.
        ConnectionInfo m_connectionCache;

        /// Nodes which have been removed since the last time changes were cleared.
        /// Use a map because we iterate.
        std::map<NodeId, std::unique_ptr<Node>> m_removedNodes;
    };

} // namespace babelwires
