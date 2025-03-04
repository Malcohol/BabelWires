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

    /// Remove all modifiers and expanded paths at and beneath a given TreeValueNode.
    class RemoveAllEditsSubcommand : public CompoundCommand<Project> {
      public:
        CLONEABLE(RemoveAllEditsSubcommand);
        RemoveAllEditsSubcommand(NodeId elementId, Path path);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_nodeId;
        Path m_path;

        // Post initialization data

        std::vector<Path> m_expandedPathsRemoved;

        struct OutgoingConnection {
            Path m_pathInSource;
            NodeId m_targetId;
            Path m_pathInTarget;
        };

        std::vector<OutgoingConnection> m_outgoingConnectionsRemoved;
    };

} // namespace babelwires