/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Features/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class Project;
    struct ModifierData;

    /// Remove a modifier from a feature element.
    /// This will not remove modifiers on array elements affected by removing
    /// an array size modifier. See RemoveModifierCommand.
    class RemoveSimpleModifierSubcommand : public SimpleCommand<Project> {
      public:
        RemoveSimpleModifierSubcommand(ElementId targetId, Path featurePath);
        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        ElementId getTargetElementId() const;
        const Path& getFeaturePath() const;

      private:
        ElementId m_targetElementId;

        Path m_featurePath;

        /// Describes the modifier which got removed.
        std::unique_ptr<ModifierData> m_modifierToRestore;
    };
} // namespace babelwires
