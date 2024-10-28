/**
 * The command which removes an entry from an array.
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

    /// Remove all modifiers and expanded paths at and beneath a given feature.
    class RemoveAllEditsSubcommand : public CompoundCommand<Project> {
      public:
        RemoveAllEditsSubcommand(ElementId elementId, Path pathToFeature);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        Path m_path;

        std::vector<Path> m_expandedPathsRemoved;

        struct OutgoingConnection {
            Path m_pathInSource;
            ElementId m_targetId;
            Path m_pathInTarget;
        };

        std::vector<OutgoingConnection> m_outgoingConnectionsRemoved;
    };

} // namespace babelwires