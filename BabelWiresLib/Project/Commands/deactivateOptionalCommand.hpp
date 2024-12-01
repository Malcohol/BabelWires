/**
 * The command which deactivates an optional in a RecordType.
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

    /// Deactivate an optional in a RecordType
    class DeactivateOptionalCommand : public CompoundCommand<Project> {
      public:
        DeactivateOptionalCommand(std::string commandName, NodeId elementId, Path featurePath,
                               ShortId optional);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_nodeId;
        Path m_pathToRecord;
        ShortId m_optional;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires
