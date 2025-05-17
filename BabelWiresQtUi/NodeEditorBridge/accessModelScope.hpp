/**
 * An AccessModelScope object provides readonly access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

namespace babelwires {

    /// Any readonly access to the model contents should be performed within the scope of an object of this type.
    /// Note: This class is intended to future proofs the system for the day when concurrency is added to the project.
    class AccessModelScope final {
      public:
        AccessModelScope(const ProjectGraphModel& bridge);
        ~AccessModelScope();

        const CommandManager<Project>& getCommandManager() const;
        const Project& getProject() const;

      protected:
        const ProjectGraphModel& m_projectGraphModel;
    };

} // namespace babelwires
