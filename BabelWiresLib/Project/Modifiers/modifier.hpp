/**
 * A Modifier changes the value in a FeatureElement, and corresponds to a user edit.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/pathStep.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Utilities/enumFlags.hpp>
#include <Common/types.hpp>


#include <memory>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    struct ModifierData;
    class Project;
    class ValueTreeNode;
    class Path;
    class FeatureElement;
    class ConnectionModifier;

    /// A Modifier changes a value somewhere in the input of a FeatureElement, and corresponds to a user edit.
    class Modifier : public Cloneable {
      public:
        CLONEABLE_ABSTRACT(Modifier);
        DOWNCASTABLE_TYPE_HIERARCHY(Modifier);

        Modifier(std::unique_ptr<ModifierData> modifierData);
        Modifier(const Modifier& other);
        virtual ~Modifier();

        /// Describes whether the modifier succeeded or the type of failure.
        enum class State : unsigned int {
            Success,
            TargetMissing,
            /// Applicable only to connection modifiers.
            SourceMissing,
            ApplicationFailed
        };

        /// Did the modifier succeed or else, which type of failure was there?
        State getState() const;

        /// Get a description of the failure.
        /// Returns the empty string if the modifier succeeded.
        std::string getReasonForFailure() const;

        /// Get the owner or nullptr, if this modifier is not currently owned.
        const FeatureElement* getOwner() const;

        /// Set or clear the owner of this modifier.
        void setOwner(FeatureElement* newOwner);

        ModifierData& getModifierData();
        const ModifierData& getModifierData() const;

        /// Convenience function for finding the node in the valueTree affected by this modifier.
        const Path& getTargetPath() const;

        /// If the modifier is a local modifier, apply it. Otherwise, do nothing.
        virtual void applyIfLocal(UserLogger& userLogger, ValueTreeNode* container);

        /// Returns this if this modifier is a connection modifier, otherwise returns nullptr;
        const ConnectionModifier* asConnectionModifier() const;
        ConnectionModifier* asConnectionModifier();

        /// If the modifier modified some data, set it back to the default.
        void unapply(ValueTreeNode* container) const;

        // clang-format off
        /// Describes the way a modifier may have changed.
        enum class Changes : unsigned int
        {
            NothingChanged            = 0b0000000,
            
            ModifierIsNew             = 0b0000001,
            ModifierFailed            = 0b0000010,
            ModifierRecovered         = 0b0000100,
    
            /// This is not used during processing, but may be used by an UI.
            /// If a modifier is moved, a clone of its old state added to a
            /// FeatureElement's removedModifiers, also flagged with ModifierMoved.
            ModifierMoved             = 0b0100000,
    
            // Only apply to ConnectionModifiers:
    
            ModifierConnected         = 0b0001000,
            ModifierDisconnected      = 0b0010000,
    
            ModifierChangesMask       = 0b0111111,
        };
        // clang-format on

        /// Query the modifier for any of the given changes.
        bool isChanged(Changes changes) const;

        /// Clear any changes the modifier is carrying.
        void clearChanges();

        /// Adjust the pathToFeature of this modifier by the adjustment.
        /// Asserts that the path leads into the array and that its
        /// index is greater than startIndex.
        void adjustArrayIndex(const babelwires::Path& pathToArray, babelwires::ArrayIndex startIndex,
                              int adjustment);

      public:
        // Convenience methods.

        /// Convenience method: Did the modifier fail?
        bool isFailed() const { return getState() != State::Success; }

      protected:
        void setSucceeded();
        void setFailed(State failureState, std::string reasonForFailure);

        /// Set flags recording a change.
        void setChanged(Changes changes);

        /// Get the owner or nullptr, if this modifier is not currently owned.
        FeatureElement* getOwner();

        virtual const ConnectionModifier* doAsConnectionModifier() const = 0;

      private:
        std::string m_reasonForFailure;

        /// The data of the operation.
        std::unique_ptr<ModifierData> m_data;

        /// The featureElement which owns this modifier.
        FeatureElement* m_owner = nullptr;

        /// The type of failure, or success.
        State m_state = State::Success;

        /// The accumulated change since the last time they were cleared.
        Changes m_changes = Changes::ModifierIsNew;
    };

    DEFINE_ENUM_FLAG_OPERATORS(Modifier::Changes);

} // namespace babelwires
