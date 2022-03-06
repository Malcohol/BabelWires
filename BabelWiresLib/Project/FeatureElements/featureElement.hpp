/**
 * FeatureElements are the fundimental constituent of the project.
 * They expose input and output Features, and carry edits.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/FeatureElements/contentsCache.hpp"
#include "BabelWiresLib/Project/FeatureElements/editTree.hpp"
#include "BabelWiresLib/Project/projectIds.hpp"

#include "Common/Utilities/enumFlags.hpp"
#include "Common/Utilities/pointerRange.hpp"

#include <memory>
#include <vector>

namespace babelwires {
    struct UserLogger;
    class Project;
    class Modifier;
    struct ModifierData;
    class ConnectionModifier;
    struct ElementData;
    class RecordFeature;
    class FeaturePath;
    struct UiPosition;
    struct UiSize;
    struct ProjectContext;
    class RootFeature;

    /// The fundimental constituent of the project.
    /// FeatureElements expose input and output Features, and carry edits.
    class FeatureElement {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(FeatureElement);

        FeatureElement(const ElementData& data, ElementId newId);
        virtual ~FeatureElement();

        /// Did the feature element fail, either because of an internal failure
        /// or because it is in a dependency loop?
        bool isFailed() const;

        /// Get a description of the failure.
        std::string getReasonForFailure() const;

        virtual RootFeature* getInputFeature();
        virtual RootFeature* getOutputFeature();

        const RootFeature* getInputFeature() const;
        const RootFeature* getOutputFeature() const;

        /// Get a description of the type of element (e.g. format name).
        virtual std::string getLabel() const;

        ElementId getElementId() const;
        const ElementData& getElementData() const;

        /// Find the modifier at the path, if there is one.
        Modifier* findModifier(const FeaturePath& featurePath);
        const Modifier* findModifier(const FeaturePath& featurePath) const;

        /// Client code should not call this directly, but via the project.
        Modifier* addModifier(UserLogger& userLogger, const ModifierData& modifier);

        /// Remove the modifier at the path.
        /// Client code should not call this directly, but via the project.
        void removeModifier(Modifier* modifier);

        /// Is the feature at the path expanded?
        bool isExpanded(const FeaturePath& featurePath) const;

        /// Set the feature at the path to be expanded or not.
        void setExpanded(const FeaturePath& featurePath, bool expanded);

        /// Get a clone of the element data with the modifiers, if there are any, re-attached.
        /// Expanded paths which do not currently lead to a feature are not included.
        std::unique_ptr<babelwires::ElementData> extractElementData() const;

        const UiPosition& getUiPosition() const;
        void setUiPosition(const UiPosition& newPosition);

        const UiSize& getUiSize() const;
        void setUiSize(const UiSize& newSize);

        /// Update state of the feature and feature caches if there are changes.
        void process(UserLogger& userLogger);

        // clang-format off
        /// Describes the way an element may have changed.
        enum class Changes : unsigned int
        {
          NothingChanged                 = 0b000000000000000000,

          FeatureValueChanged            = 0b000000000000000001,
          FeatureStructureChanged        = 0b000000000000000010,
          FeatureChangesMask             = 0b000000000000000011,

          FeatureElementIsNew            = 0b000000000000000100,
          FeatureElementFailed           = 0b000000000000001000,
          FeatureElementRecovered        = 0b000000000000010000,
          FeatureElementLabelChanged     = 0b000000000000100000,
          FeatureElementChangesMask      = 0b000000000000111100,

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

        /// Query the feature element for changes.
        bool isChanged(Changes changes) const;

        /// Clear any changes the element is carrying.
        void clearChanges();

        /// Access the contained feature cache.
        const ContentsCache& getContentsCache() const;

        const EditTree& getEdits() const { return m_edits; }
        EditTree& getEdits() { return m_edits; }

        auto getConnectionModifiers() { return m_edits.modifierRange<ConnectionModifier>(); }
        auto getConnectionModifiers() const { return m_edits.modifierRange<ConnectionModifier>(); }

        auto getRemovedModifiers() const { return PointerRange(m_removedModifiers); }

        /// Is this feature element currently in a dependency loop.
        bool isInDependencyLoop() const;

        /// Set or unset the fact that this feature element is in a dependency loop.
        void setInDependencyLoop(bool isInLoop);

        /// Adjust modifiers which refer to an array at the path, starting at the startIndex.
        void adjustArrayIndices(const babelwires::FeaturePath& pathToArray, babelwires::ArrayIndex startIndex,
                                int adjustment);

      protected:
        virtual void doProcess(UserLogger& userLogger) = 0;

        void clearInternalFailure();
        void setInternalFailure(std::string reasonForFailure);

        void setFactoryName(std::string factoryName);
        void setFactoryName(LongIdentifier identifier);

        friend Modifier;
        friend ConnectionModifier;

        /// Set flags recording a change.
        void setChanged(Changes changes);

        /// State that the target feature (or source, if a connection modifier) of this modifier
        /// has been adjusted.
        /// Array addition/removal could be handled by adding and removing elements. However, that
        /// could triggering lots of unnecessary processing.
        /// Flags this element as having a moved modifier AND makes a copy of the old state of the modifier
        /// in m_removedModifiers.
        void setModifierMoving(const Modifier& modifierAboutToMove);

        ElementData& getElementData();

        /// Add the modifier, but don't apply it yet.
        /// Returns the new modifier.
        Modifier* addModifierWithoutApplyingIt(const ModifierData& modifier);

        /// Apply the element's local modifiers.
        friend babelwires::ElementData;
        void applyLocalModifiers(UserLogger& userLogger);

      private:
        std::string m_internalFailure;
        bool m_isInDependencyLoop = false;

        std::unique_ptr<ElementData> m_data;

        /// The factory name is kept as a member, because we have to query it from the project context,
        /// which is not available when the label is queried.
        // TODO Move into ElementData.
        std::string m_factoryName;

        /// Container for the edits applied to this feature element.
        EditTree m_edits;

        /// This is cleared when changes are cleared.
        std::vector<std::unique_ptr<Modifier>> m_removedModifiers;

        /// The accumulated change since the last time they were cleared.
        Changes m_changes = Changes::FeatureElementIsNew;

      protected:
        ContentsCache m_contentsCache;
    };

    DEFINE_ENUM_FLAG_OPERATORS(FeatureElement::Changes);

} // namespace babelwires
