/**
 * The command which adds a modifier to a Node.
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

    /// Add a modifier to a Node.
    class AddModifierCommand : public CompoundCommand<Project> {
      public:
        CLONEABLE(AddModifierCommand);
        AddModifierCommand(std::string commandName, NodeId targetId, std::unique_ptr<ModifierData> modifierToAdd);
        AddModifierCommand(const AddModifierCommand& other);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_targetNodeId;

        /// Describes the modifier which will be added.
        std::unique_ptr<ModifierData> m_modifierToAdd;
    };

} // namespace babelwires
