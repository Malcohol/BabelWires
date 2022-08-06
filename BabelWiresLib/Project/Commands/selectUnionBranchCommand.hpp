/**
 * The command which activates optionals in a RecordWithOptionalsFeature.
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
    class ModifierData;

    /// Activate an optional in a RecordWithOptionalsFeature
    class SelectUnionBranchCommand : public CompoundCommand<Project> {
      public:
        SelectUnionBranchCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               Identifier tagToSelect);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToUnion;
        Identifier m_tagToSelect;

        std::unique_ptr<ModifierData> m_unionModifierToAdd;
        std::unique_ptr<ModifierData> m_unionModifierToRemove;
    };
} // namespace babelwires
