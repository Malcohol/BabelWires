/**
 * The command which sets the size of an array.
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

    /// Set the size of an array.
    class SetArraySizeCommand : public CompoundCommand<Project> {
      public:
        SetArraySizeCommand(std::string commandName, NodeId elementId, Path featurePath,
                                    int newSize);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        void executeBody(Project& project) const;
        
      private:
        NodeId m_nodeId;
        Path m_pathToArray;
        unsigned int m_newSize;
        unsigned int m_oldSize;

        /// Did an old modifier get replaced (otherwise this is the first modification).
        bool m_wasModifier = false;
    };

} // namespace babelwires
