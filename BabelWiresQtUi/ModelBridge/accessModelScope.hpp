/**
 * An AccessModelScope object provides readonly access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

namespace babelwires {

    /// Any readonly access to the model contents should be performed within the scope of an object of this type.
    /// Note: This class is intended to future proofs the system for the day when concurrency is added to the project.
    class AccessModelScope {
      public:
        AccessModelScope(ProjectBridge& bridge);
        virtual ~AccessModelScope();

        const CommandManager<Project>& getCommandManager();
        const Project& getProject();

      protected:
        ProjectBridge& m_projectBridge;
    };

} // namespace babelwires
