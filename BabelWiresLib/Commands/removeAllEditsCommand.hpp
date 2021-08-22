/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Commands/commands.hpp"

namespace babelwires {

    /// Remove all modifiers and expanded paths at and beneath a given feature.
    class RemoveAllEditsCommand : public SimpleCommand {
      public:
        /// If indexOfEntryToRemove is negative, the element is removed from the end.
        RemoveAllEditsCommand(std::string commandName, ElementId elementId, FeaturePath pathToFeature);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToFeature;

        std::vector<std::unique_ptr<ModifierData>> m_modifiersRemoved;
        std::vector<FeaturePath> m_expandedPathsRemoved;

        struct OutgoingConnection {
            FeaturePath m_pathInSource;
            ElementId m_targetId;
            FeaturePath m_pathInTarget;
        };

        std::vector<OutgoingConnection> m_outgoingConnectionsRemoved;
    };

} // namespace babelwires