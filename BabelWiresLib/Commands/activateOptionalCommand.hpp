/**
 * The command which activates optionals in a RecordWithOptionalsFeature.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWiresLib/Commands/commands.hpp"

namespace babelwires {

    /// Activate an optional in a RecordWithOptionalsFeature
    class ActivateOptionalCommand : public SimpleCommand<Project> {
      public:
        ActivateOptionalCommand(std::string commandName, ElementId elementId, FeaturePath featurePath,
                               Identifier optional);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToRecord;
        Identifier m_optional;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires
