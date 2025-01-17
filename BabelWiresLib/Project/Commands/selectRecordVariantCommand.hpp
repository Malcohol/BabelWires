/**
 * The command which selects the variant in a RecordWithVariantType.
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

    /// Activate an optional in a RecordWithVariantType
    class SelectRecordVariantCommand : public CompoundCommand<Project> {
      public:
        SelectRecordVariantCommand(std::string commandName, NodeId elementId, Path featurePath,
                               ShortId tagToSelect);
        virtual ~SelectRecordVariantCommand();

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_nodeId;
        Path m_pathToRecord;
        ShortId m_tagToSelect;

        std::unique_ptr<ModifierData> m_recordModifierToAdd;
        std::unique_ptr<ModifierData> m_recordModifierToRemove;
    };
} // namespace babelwires
