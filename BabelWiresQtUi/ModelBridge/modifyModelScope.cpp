/**
 * A ModifyModelScope object provides write access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Commands/commands.hpp>

babelwires::ModifyModelScope::ModifyModelScope(ProjectBridge& bridge)
    : AccessModelScope(bridge) {}

babelwires::ModifyModelScope::~ModifyModelScope() {
    m_projectBridge.processAndHandleModelChanges();
}

babelwires::Project& babelwires::ModifyModelScope::getProject() {
    return m_projectBridge.m_project;
}

babelwires::CommandManager<babelwires::Project>& babelwires::ModifyModelScope::getCommandManager() {
    return m_projectBridge.m_commandManager;
}
