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
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    class Project;

    /// Set the size of an array.
    class SetTypeVariableCommand : public CompoundCommand<Project> {
      public:
        CLONEABLE(SetTypeVariableCommand);
        SetTypeVariableCommand(std::string commandName, NodeId nodeId, Path pathToGenericType, unsigned int variableIndex,
                                    TypeRef newType);

        virtual bool initializeAndExecute(Project& project) override;

      private:
        NodeId m_nodeId;
        Path m_pathToGenericType;
        unsigned int m_variableIndex;
        TypeRef m_newType;
    };

} // namespace babelwires
