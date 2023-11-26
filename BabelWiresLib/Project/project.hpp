/**
 * The Project manages the graph of FeatureElements, and propagates data from sources to targets.
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

    class RecordFeature;
    class TargetFileFormatRegistry;
    class SourceFileFormatRegistry;
    class ProcessorFactoryRegistry;
    struct ProjectContext;
    class Modifier;
    struct ModifierData;
    class FeatureElement;
    struct ElementData;
    class ConnectionModifier;
    class FeaturePath;
    struct UiPosition;
    struct UiSize;

    /// The Project manages the graph of FeatureElements, and propagates data from sources to targets.
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
        ElementId addFeatureElement(const ElementData& data);

        /// Remove the element with the given id.
        void removeElement(ElementId id);

        /// Add a modifier as described by the data.
        void addModifier(ElementId elementId, const ModifierData& modifierData);

        /// Remove the modifier at the path from the given element.
        void removeModifier(ElementId elementId, const FeaturePath& featurePath);

        /// Add an element to the array at the given path.
        /// Modifiers below the index are adjusted.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        /// This is special-cased to avoid treating all affected modifiers as add/removed,
        /// and possibly triggering arbitrary amount of work.
        // TODO: Much of the work here could be pulled out into the commands. Same with other operations below.
        void addArrayEntries(ElementId elementId, const FeaturePath& featurePath, int indexOfNewElement,
                             int numEntriesToAdd, bool ensureModifier);

        /// Remove an element from the array at the given path.
        /// This will adjust the paths in modifiers which are affected.
        /// However, note that this will not remove modifiers.
        /// If ensureModifier is true, then a modifier will be added if not present.
        /// Otherwise, a modifier will be removed if present.
        void removeArrayEntries(ElementId elementId, const FeaturePath& featurePath, int indexOfElementToRemove,
                                int numEntriesToRemove, bool ensureModifier);

        /// Adjust modifiers and connections which point into an array to adapt to shifted array elements.
        /// If adjustment is positive, then modifiers at and above startIndex are modified.
        /// If adjustment is negative, then modifiers at (startIndex - adjustment) and above are modified.
        void adjustModifiersInArrayElements(ElementId elementId, const FeaturePath& pathToArray, ArrayIndex startIndex,
                                            int adjustment);

        /// Activate a modifier in a RecordWithOptionalsFeature.
        void activateOptional(ElementId elementId, const FeaturePath& pathToRecord, ShortId optional,
                              bool ensureModifier);

        /// Deactivate a modifier in a RecordWithOptionalsFeature.
        /// Note that this will not remove modifiers.
        void deactivateOptional(ElementId elementId, const FeaturePath& pathToRecord, ShortId optional,
                                bool ensureModifier);

        /// Set the Ui position of the element.
        void setElementPosition(ElementId elementId, const UiPosition& newPosition);

        /// Sets the Ui size of the element's contents.
        void setElementContentsSize(ElementId elementId, const UiSize& newSize);

        const TargetFileFormatRegistry& getFactoryFormatRegistry() const;
        const SourceFileFormatRegistry& getFileFormatRegistry() const;

        FeatureElement* getFeatureElement(ElementId id);
        const FeatureElement* getFeatureElement(ElementId id) const;

        /// Reload the source file.
        /// File exceptions are caught and written to the userLogger.
        void tryToReloadSource(ElementId id);

        /// Save the target file non-interactively.
        /// File exceptions are caught and written to the userLogger.
        void tryToSaveTarget(ElementId id);

        /// Reload all the source file.
        /// File exceptions are caught and written to the userLogger.
        void tryToReloadAllSources();

        /// Save all the target files non-interactively.
        /// File exceptions are caught and written to the userLogger.
        void tryToSaveAllTargets();

        ///
        const std::map<ElementId, std::unique_ptr<FeatureElement>>& getElements() const;

        /// Process any changes in the whole project.
        void process();

        /// Mark all features in the project as unchanged.
        void clearChanges();

        /// Information about the connections between elements.
        /// The connectionInfo includes elements and modifiers which failed.
        struct ConnectionInfo {
            using Connections = std::vector<std::tuple<ConnectionModifier*, FeatureElement*>>;
            using ConnectionMap = std::unordered_map<const FeatureElement*, Connections>;

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
        const std::map<ElementId, std::unique_ptr<FeatureElement>>& getRemovedElements() const;

        /// Get the ProjectId of the current project.
        ProjectId getProjectId() const;

        /// If any of the ids are unavailable, replace them by currently available ones.
        void updateWithAvailableIds(std::vector<ElementId>& idsInOut) const;

      private:
        /// Return an available id, using the provided hint if it is valid and available.
        ElementId reserveElementId(ElementId hint);

        /// Mark the connection cache as invalid, so the next time it is queried, it gets recomputed.
        void setConnectionCacheInvalid();

        /// If the output feature of e has any changes, propagate them to the input features of connected
        /// elements, as described by the requiredForMap.
        void propagateChanges(const FeatureElement* e);

        /// Set the ProjectId to a random value.
        void randomizeProjectId();

        void addConnectionToCache(FeatureElement* element, ConnectionModifier* data);

        void removeConnectionFromCache(FeatureElement* element, ConnectionModifier* data);

        void validateConnectionCache() const;

        FeatureElement* addFeatureElementWithoutCachingConnection(const ElementData& data);
        void addFeatureElementConnectionsToCache(FeatureElement* element);

      private:
        ProjectContext& m_context;

        /// The userLogger is only used for logging user-visible errors, warnings and messages.
        /// It is not intended for debug logging.
        UserLogger& m_userLogger;

        /// The ID of the current project. Randomly assigned on construction and clear.
        ProjectId m_projectId;

        /// 0 == INVALID_ELEMENT_ID and is never used for an actual element.
        ElementId m_maxAssignedElementId = 0;

        /// A map of elements, keyed by ElementID.
        std::map<ElementId, std::unique_ptr<FeatureElement>> m_featureElements;

        /// Cache of connection information.
        ConnectionInfo m_connectionCache;

        /// Feature elements which have been removed since the last time changes were cleared.
        /// Use a map because we iterate.
        std::map<ElementId, std::unique_ptr<FeatureElement>> m_removedFeatureElements;
    };

} // namespace babelwires
