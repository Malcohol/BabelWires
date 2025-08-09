/**
 * The command which adds a connection between Nodes.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class Project;
    struct ConnectionModifierData;
    class SetTypeVariableModifierData;

    /// Add a modifier to a Node.
    class AddConnectionCommand : public CompoundCommand<Project> {
      public:
        ~AddConnectionCommand();

        CLONEABLE(AddConnectionCommand);
        AddConnectionCommand(std::string commandName, NodeId targetId, std::unique_ptr<ConnectionModifierData> modifierToAdd);
        AddConnectionCommand(const AddConnectionCommand& other);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_targetNodeId;

        /// Describes the modifier which will be added.
        std::unique_ptr<ConnectionModifierData> m_modifierToAdd;

        /// 
        std::unique_ptr<SetTypeVariableModifierData> m_typeAssignmentModifier;
    };

} // namespace babelwires
