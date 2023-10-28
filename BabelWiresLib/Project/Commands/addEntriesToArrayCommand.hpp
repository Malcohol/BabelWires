/**
 * The command which adds entries to arrays.
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

    /// Add an element to an array feature.
    class AddEntriesToArrayCommand : public SimpleCommand<Project> {
      public:
        AddEntriesToArrayCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               unsigned int indexOfNewEntries, unsigned int numEntriesToAdd = 1);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToArray;
        unsigned int m_indexOfNewEntries;
        unsigned int m_numEntriesToAdd;

        /// All arrays in the project whose structure derives from this array and therefore need to be adjusted.
        std::vector<std::tuple<ElementId, FeaturePath>> m_indirectlyAffectedArrays;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires