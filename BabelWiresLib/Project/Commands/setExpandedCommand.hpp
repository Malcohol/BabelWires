/**
 * The command which expands or collapses a Feature of a FeatureElement.
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

    class SetExpandedCommand : public SimpleCommand<Project> {
      public:
        SetExpandedCommand(std::string commandName, ElementId elementId, FeaturePath pathToCompound, bool expanded);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToCompound;
        bool m_expanded;
    };

} // namespace babelwires
