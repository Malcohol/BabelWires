/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"

namespace babelwires {

    /// Remove an element from an array feature.
    class RemoveEntryFromArrayCommand : public SimpleCommand {
      public:
        /// If indexOfEntryToRemove is negative, the element is removed from the end.
        RemoveEntryFromArrayCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                                    int indexOfEntryToRemove, int numEntriesToRemove);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToArray;
        int m_indexOfEntryToRemove;
        int m_numEntriesToRemove;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;

        std::vector<std::unique_ptr<ModifierData>> m_modifiersRemovedFromEntry;
        std::vector<FeaturePath> m_expandedPathsInRemovedEntries;

        struct OutgoingConnection {
            FeaturePath m_pathInSource;
            ElementId m_targetId;
            FeaturePath m_pathInTarget;
        };

        std::vector<OutgoingConnection> m_outgoingConnections;
    };

} // namespace babelwires