/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWiresLib/Commands/commands.hpp"
#include "BabelWiresLib/Features/Path/featurePath.hpp"
#include "BabelWiresLib/Project/projectIds.hpp"

namespace babelwires {
    class Project;

    /// Add an element to an array feature.
    class AddEntryToArrayCommand : public SimpleCommand<Project> {
      public:
        AddEntryToArrayCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               unsigned int indexOfNewEntry);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToArray;
        unsigned int m_indexOfNewEntry;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires