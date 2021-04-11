/**
 * The command which pastes content into a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"
#include "BabelWires/Project/projectData.hpp"

#include "BabelWires/ProjectExtra/connectionDescription.hpp"

namespace babelwires {

    /// The command which pastes content into a project.
    class PasteElementsCommand : public SimpleCommand {
      public:
        PasteElementsCommand(std::string commandName, ProjectData dataToPaste);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ProjectData m_dataToPaste;

        ProjectId m_idOfSourceProject;

        /// We keep connections separately, since it makes undo easier.
        std::vector<ConnectionDescription> m_connectionsToPaste;
    };

} // namespace babelwires
