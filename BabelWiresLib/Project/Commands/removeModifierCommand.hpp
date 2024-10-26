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

    /// Remove the modifier, and restore any other modifiers removed when an array's size changes.
    class RemoveModifierCommand : public CompoundCommand<Project> {
      public:
        RemoveModifierCommand(std::string commandName, ElementId targetId, Path featurePath);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        ElementId m_elementId;
        Path m_featurePath;
    };

} // namespace babelwires
