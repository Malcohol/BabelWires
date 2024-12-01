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
        ChangeFileCommand(std::string commandName, ElementId elementId, std::filesystem::path newFilePath);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        std::filesystem::path m_newFilePath;
        std::filesystem::path m_oldFilePath;
    };

} // namespace babelwires
