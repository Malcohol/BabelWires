/**
 * The command which activates optionals in a RecordType.
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

    /// Activate an optional in a RecordType
    class ActivateOptionalCommand : public SimpleCommand<Project> {
      public:
        CLONEABLE(ActivateOptionalCommand);
        ActivateOptionalCommand(std::string commandName, NodeId nodeId, Path pathToRecord, ShortId optional);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        NodeId m_nodeId;
        Path m_pathToRecord;
        ShortId m_optional;

        // Post initialization data

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires
