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

    /// Adjust modifiers and connections which point into an array to adapt to shifted array elements.
    class AdjustModifiersInArraySubcommand : public CompoundCommand<Project> {
      public:
        /// If adjustments is negative, then the range startIndex to (startIndex - adjustment) is considered as being
        /// removed.
        AdjustModifiersInArraySubcommand(ElementId elementId,
                                          const babelwires::FeaturePath& pathToArray, babelwires::ArrayIndex startIndex,
                                          int adjustment);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToArray;
        unsigned int m_startIndex;
        int m_adjustment;
    };

} // namespace babelwires