/**
 * Commands which removes modifiers from an node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class Project;
    struct ModifierData;

    /// Remove the modifier, and restore any other modifiers removed when an array's size changes.
    class RemoveModifierCommand : public CompoundCommand<Project> {
      public:
        CLONEABLE(RemoveModifierCommand);
        RemoveModifierCommand(std::string commandName, NodeId targetId, Path featurePath);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        NodeId m_nodeId;
        Path m_path;
    };

} // namespace babelwires
