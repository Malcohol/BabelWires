/**
 * The command which removes an entry from an array.
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
    struct ModifierData;

    /// Remove all modifiers and expanded paths at and beneath a given feature.
    class RemoveAllEditsSubcommand : public SimpleCommand<Project> {
      public:
        RemoveAllEditsSubcommand(std::string commandName, ElementId elementId, FeaturePath pathToFeature);

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