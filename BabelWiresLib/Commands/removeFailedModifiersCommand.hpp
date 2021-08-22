/**
 * The command which removes all failed modifiers at or beneath the given path.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Commands/commands.hpp"

namespace babelwires {

    /// Remove all failed modifiers at or beneath the given path.
    class RemoveFailedModifiersCommand : public CompoundCommand {
      public:
        RemoveFailedModifiersCommand(std::string commandName, ElementId targetId, FeaturePath featurePath);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        ElementId m_targetId;
        FeaturePath m_featurePath;
    };

} // namespace babelwires
