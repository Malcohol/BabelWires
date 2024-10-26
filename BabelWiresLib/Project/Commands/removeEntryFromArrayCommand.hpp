/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class Project;

    /// Remove an element from an array feature.
    class RemoveEntryFromArrayCommand : public CompoundCommand<Project> {
      public:
        RemoveEntryFromArrayCommand(std::string commandName, ElementId elementId, Path featurePath,
                                    unsigned int indexOfEntryToRemove, unsigned int numEntriesToRemove);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        Path m_pathToArray;
        unsigned int m_indexOfEntryToRemove;
        unsigned int m_numEntriesToRemove;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
        bool m_isSubcommand;
    };

} // namespace babelwires