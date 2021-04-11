/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"

namespace babelwires {

    /// Remove a modifier from a feature element.
    /// This will not remove modifiers on array elements affected by removing
    /// an array size modifier. See RemoveModifierCommand.
    class RemoveSimpleModifierCommand : public SimpleCommand {
      public:
        RemoveSimpleModifierCommand(std::string commandName, ElementId targetId, FeaturePath featurePath);
        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        ElementId getTargetElementId() const;
        const FeaturePath& getFeaturePath() const;

      private:
        ElementId m_targetElementId;

        FeaturePath m_featurePath;

        /// Describes the modifier which got removed.
        std::unique_ptr<ModifierData> m_modifierToRestore;
    };

    /// Remove the modifier, and restore any other modifiers removed when an array's size changes.
    class RemoveModifierCommand : public CompoundCommand {
      public:
        RemoveModifierCommand(std::string commandName, ElementId targetId, FeaturePath featurePath);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        std::unique_ptr<RemoveSimpleModifierCommand> m_simpleCommand;
    };

} // namespace babelwires
