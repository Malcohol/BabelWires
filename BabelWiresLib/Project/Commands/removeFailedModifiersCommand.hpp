/**
 * The command which removes all failed modifiers at or beneath the given path.
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

    /// Remove all failed modifiers at or beneath the given path.
    class RemoveFailedModifiersCommand : public CompoundCommand<Project> {
      public:
        RemoveFailedModifiersCommand(std::string commandName, ElementId targetId, Path featurePath);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        ElementId m_targetId;
        Path m_targetPath;
    };

} // namespace babelwires
