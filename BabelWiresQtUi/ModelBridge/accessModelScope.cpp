/**
 * An AccessModelScope object provides readonly access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/accessModelScope.hpp"

babelwires::AccessModelScope::AccessModelScope(ProjectBridge& bridge)
    : m_projectBridge(bridge) {}

babelwires::AccessModelScope::~AccessModelScope() {}

const babelwires::Project& babelwires::AccessModelScope::getProject() {
    return m_projectBridge.m_project;
}

const babelwires::CommandManager<babelwires::Project>& babelwires::AccessModelScope::getCommandManager() {
    return m_projectBridge.m_commandManager;
}
