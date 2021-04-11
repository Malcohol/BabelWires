/**
 * The command which adds a modifier to a feature element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWires/Commands/commands.hpp"

namespace babelwires {

    /// Add a modifier to a feature element.
    class AddModifierCommand : public SimpleCommand {
      public:
        AddModifierCommand(std::string commandName, ElementId targetId, std::unique_ptr<ModifierData> modifierToAdd);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_targetElementId;

        /// Describes the modifier which will be added.
        std::unique_ptr<ModifierData> m_modifierToAdd;

        /// Describes the modifier which got replaced, if there was one.
        std::unique_ptr<ModifierData> m_modifierToRemove;
    };

} // namespace babelwires
