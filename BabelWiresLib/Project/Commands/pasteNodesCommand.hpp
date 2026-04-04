/**
 * The command which pastes content into a project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>

namespace babelwires {

    /// The command which pastes content into a project.
    class BABELWIRESLIB_API PasteNodesCommand : public SimpleCommand<Project> {
      public:
        DOWNCASTABLE(PasteNodesCommand, SimpleCommand<Project>);
        CLONEABLE(PasteNodesCommand);
        PasteNodesCommand(std::string commandName, ProjectData dataToPaste);

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
