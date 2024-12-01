/**
 * The command which expands or collapses a ValueTreeNode.
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

    class SetExpandedCommand : public SimpleCommand<Project> {
      public:
        SetExpandedCommand(std::string commandName, NodeId elementId, Path pathToCompound, bool expanded);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_elementId;
        Path m_pathToCompound;
        bool m_expanded;
    };

} // namespace babelwires
