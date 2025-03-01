/**
 * The command which changes the file of a FileNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

#include <filesystem>

namespace babelwires {
    class Project;

    /// Change the source file of a FileNode.
    class ChangeFileCommand : public SimpleCommand<Project> {
      public:
        CLONEABLE(ChangeFileCommand);
        ChangeFileCommand(std::string commandName, NodeId nodeId, std::filesystem::path newFilePath);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_nodeId;
        std::filesystem::path m_newFilePath;

        // Post initialization data

        std::filesystem::path m_oldFilePath;
    };

} // namespace babelwires
