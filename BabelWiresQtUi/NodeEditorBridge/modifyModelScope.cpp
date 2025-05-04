/**
 * A ModifyModelScope object provides write access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Commands/commands.hpp>

babelwires::ModifyModelScope::ModifyModelScope(ProjectGraphModel& bridge)
    : m_projectGraphModel(bridge) {}

babelwires::ModifyModelScope::~ModifyModelScope() {
    m_projectGraphModel.processAndHandleModelChanges();
}

const babelwires::Project& babelwires::ModifyModelScope::getProject() const {
    return m_projectGraphModel.m_project;
}

babelwires::Project& babelwires::ModifyModelScope::getProject() {
    return m_projectGraphModel.m_project;
}

const babelwires::CommandManager<babelwires::Project>& babelwires::ModifyModelScope::getCommandManager() const {
    return m_projectGraphModel.m_commandManager;
}

babelwires::CommandManager<babelwires::Project>& babelwires::ModifyModelScope::getCommandManager() {
    return m_projectGraphModel.m_commandManager;
}
