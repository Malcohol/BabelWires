/**
 * A ModifyModelScope object provides write access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

namespace babelwires {

    /// Any changes to the model should be performed within the scope of an object of this type.
    /// Note: This class is intended to future proofs the system for the day when concurrency is added to the project.
    class ModifyModelScope : public AccessModelScope {
      public:
        ModifyModelScope(ProjectGraphModel& bridge);
        virtual ~ModifyModelScope();

        const CommandManager<Project>& getCommandManager() const;
        CommandManager<Project>& getCommandManager();

        const Project& getProject() const;
        Project& getProject();

      private:
        ProjectGraphModel::State m_previousState;
        ProjectGraphModel& m_projectGraphModel;
    };

} // namespace babelwires
