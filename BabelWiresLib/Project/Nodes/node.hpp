/**
 * Nodes are the fundimental constituent of the project.
 * They expose input and output Features, and carry edits.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/contentsCache.hpp>
#include <BabelWiresLib/Project/Nodes/editTree.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <Common/Utilities/enumFlags.hpp>
#include <Common/Utilities/pointerRange.hpp>

#include <memory>
#include <vector>

namespace babelwires {
    struct UserLogger;
    class Project;
    class Modifier;
    struct ModifierData;
    class ConnectionModifier;
    struct NodeData;
    class Path;
    struct UiPosition;
    struct UiSize;
    struct ProjectContext;

    /// The fundimental constituent of the project.
    /// Nodes expose input and output Features, and carry edits.
    class Node {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Node);

        Node(const NodeData& data, NodeId newId);
        virtual ~Node();

        /// Did the Node fail, either because of an internal failure
        /// or because it is in a dependency loop?
        bool isFailed() const;

        /// Get a description of the failure.
        std::string getReasonForFailure() const;

        /// Get a non-const pointer to the input feature and obtain the right to modify the feature at the given path.
        /// Returns nullptr if the modifier will be applied later anyway, so there's no
        /// work for the caller to do.
        /// This does not attempt to deal with errors, so it returns the feature if the path cannot be followed.
        ValueTreeNode* getInputNonConst(const Path& pathToModify);

        virtual const ValueTreeNode* getInput() const;
        virtual const ValueTreeNode* getOutput() const;

        /// Get a description of the type of Node (e.g. format name).
        virtual std::string getLabel() const;

        NodeId getNodeId() const;
        const NodeData& getNodeData() const;

        /// Find the modifier at the path, if there is one.
        Modifier* findModifier(const Path& featurePath);
        const Modifier* findModifier(const Path& featurePath) const;

        /// Client code should not call this directly, but via the project.
        /// Adding a modifier applies its effect, unless applyModifier is false.
        Modifier* addModifier(UserLogger& userLogger, const ModifierData& modifier, bool applyModifier = true);

        /// Remove the modifier at the path.
        /// Client code should not call this directly, but via the project.
        /// Removing a modifier undoes its effect, unless unapplyModifier is false.
        void removeModifier(Modifier* modifier, bool unapplyModifier = true);

        /// Is the feature at the path expanded?
        bool isExpanded(const Path& featurePath) const;

        /// Set the feature at the path to be expanded or not.
        void setExpanded(const Path& featurePath, bool expanded);

        /// Get a clone of the NodeData with the modifiers, if there are any, re-attached.
        /// Expanded paths which do not currently lead to a value are not included.
        std::unique_ptr<babelwires::NodeData> extractNodeData() const;

        const UiPosition& getUiPosition() const;
        void setUiPosition(const UiPosition& newPosition);

        const UiSize& getUiSize() const;
        void setUiSize(const UiSize& newSize);

        /// Update state of the feature and feature caches if there are changes.
        void process(Project& project, UserLogger& userLogger);

        // clang-format off
        /// Describes the way a Node may have changed.
        enum class Changes : unsigned int
        {
          NothingChanged                 = 0b000000000000000000,

          FeatureValueChanged            = 0b000000000000000001,
          FeatureStructureChanged        = 0b000000000000000010,
          FeatureChangesMask             = 0b000000000000000011,

          NodeIsNew            = 0b000000000000000100,
          NodeFailed           = 0b000000000000001000,
          NodeRecovered        = 0b000000000000010000,
          NodeLabelChanged     = 0b000000000000100000,
          NodeChangesMask      = 0b000000000000111100,

          ModifierAdded                  = 0b000000000001000000,
          ModifierRemoved                = 0b000000000010000000,
          ModifierFailed                 = 0b000000000100000000,
          ModifierRecovered              = 0b000000001000000000,
          ModifierConnected              = 0b000000010000000000,
          ModifierDisconnected           = 0b000000100000000000,
          ModifierMoved                  = 0b000001000000000000,
          ModifierChangesMask            = 0b000001111111000000,

          UiPositionChanged              = 0b000010000000000000,
          UiSizeChanged                  = 0b000100000000000000,

          FileChanged                    = 0b001000000000000000,

          CompoundExpandedOrCollapsed    = 0b010000000000000000,

          SomethingChanged               = 0b111111111111111111
        };
        // clang-format on

        /// Query the Node for any of the given changes.
        bool isChanged(Changes changes) const;

        /// Clear any changes the Node is carrying.
        void clearChanges();

        /// Access the contained feature cache.
        const ContentsCache& getContentsCache() const;

        const EditTree& getEdits() const { return m_edits; }
        EditTree& getEdits() { return m_edits; }

        auto getConnectionModifiers() { return m_edits.modifierRange<ConnectionModifier>(); }
        auto getConnectionModifiers() const { return m_edits.modifierRange<ConnectionModifier>(); }

        auto getRemovedModifiers() const { return PointerRange(m_removedModifiers); }

        /// Is this Node currently in a dependency loop.
        bool isInDependencyLoop() const;

        /// Set or unset the fact that this Node is in a dependency loop.
        void setInDependencyLoop(bool isInLoop);

        /// Adjust modifiers which refer to an array at the path, starting at the startIndex.
        void adjustArrayIndices(const babelwires::Path& pathToArray, babelwires::ArrayIndex startIndex,
                                int adjustment);

      protected:
        /// Get a non-const pointer to the input feature. The default implementation returns null.
        virtual ValueTreeNode* doGetInputNonConst();
        /// Get a non-const pointer to the output feature. The default implementation returns null.
        virtual ValueTreeNode* doGetOutputNonConst();
        virtual void doProcess(UserLogger& userLogger) = 0;

      protected:
        /// Update the cache and subscribe to the input for modifications.
        void setValueTrees(std::string rootLabel, ValueTreeRoot* input, const ValueTreeRoot* output);

        /// Inform the cache that modifiers may have changed.
        void updateModifierCache();

        void clearInternalFailure();
        void setInternalFailure(std::string reasonForFailure);

        void setFactoryName(std::string factoryName);
        void setFactoryName(LongId identifier);

        friend Modifier;
        friend ConnectionModifier;

        /// Set flags recording a change.
        void setChanged(Changes changes);

        /// State that the target feature (or source, if a connection modifier) of this modifier
        /// has been adjusted.
        /// Array addition/removal could be handled by adding and removing elements. However, that
        /// could triggering lots of unnecessary processing.
        /// Flags this Node as having a moved modifier AND makes a copy of the old state of the modifier
        /// in m_removedModifiers.
        void setModifierMoving(const Modifier& modifierAboutToMove);

        NodeData& getNodeData();

        /// Add the modifier, but don't apply it yet.
        /// Returns the new modifier.
        Modifier* addModifierWithoutApplyingIt(const ModifierData& modifier);

        /// Apply the Node's local modifiers.
        friend babelwires::NodeData;
        void applyLocalModifiers(UserLogger& userLogger);

        /// Obtain the right to modify the feature at the given path.
        /// This does not attempt to deal with errors, so it just returns true if the path cannot be followed.
        void modifyValueAt(ValueTreeNode* input, const Path& path);

        /// This is called by process, to signal that all modifications are finished.
        void finishModifications(const Project& project, UserLogger& userLogger);

      private:
        std::string m_internalFailure;
        bool m_isInDependencyLoop = false;

        std::unique_ptr<NodeData> m_data;

        /// The factory name is kept as a member, because we have to query it from the project context,
        /// which is not available when the label is queried.
        // TODO Move into NodeData.
        std::string m_factoryName;

        /// Container for the edits applied to this Node.
        EditTree m_edits;

        /// This is cleared when changes are cleared.
        std::vector<std::unique_ptr<Modifier>> m_removedModifiers;

        /// The accumulated change since the last time they were cleared.
        Changes m_changes = Changes::NodeIsNew;

        /// After modifications to compounds, modifiers need to be reapplied.
        /// This records the paths at which that should happen.
        std::vector<Path> m_modifiedPaths;

        ContentsCache m_contentsCache;
    };

    DEFINE_ENUM_FLAG_OPERATORS(Node::Changes);

} // namespace babelwires
