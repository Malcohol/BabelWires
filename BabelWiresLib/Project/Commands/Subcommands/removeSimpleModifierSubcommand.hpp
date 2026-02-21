/**
 * Commands which removes modifiers from a node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    class Project;
    struct ModifierData;

    /// Remove a modifier from a Node.
    /// This will not remove modifiers on array elements affected by removing
    /// an array size modifier. See RemoveModifierCommand.
    class RemoveSimpleModifierSubcommand : public SimpleCommand<Project> {
      public:
        CLONEABLE(RemoveSimpleModifierSubcommand);
        RemoveSimpleModifierSubcommand(NodeId targetId, Path featurePath);
        RemoveSimpleModifierSubcommand(const RemoveSimpleModifierSubcommand& other);
        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        NodeId getTargetNodeId() const;
        const Path& getTargetPath() const;

      private:
        NodeId m_targetNodeId;

        Path m_targetPath;

        /// Describes the modifier which got removed.
        std::unique_ptr<ModifierData> m_modifierToRestore;
    };
} // namespace babelwires
