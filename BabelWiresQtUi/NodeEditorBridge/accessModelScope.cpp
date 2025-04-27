/**
 * An AccessModelScope object provides readonly access to the Project within the scope of its lifetime.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>

babelwires::AccessModelScope::AccessModelScope(ProjectBridge& bridge)
    : m_projectGraphModel(bridge) {}

babelwires::AccessModelScope::~AccessModelScope() {}

const babelwires::Project& babelwires::AccessModelScope::getProject() {
    return m_projectGraphModel.m_project;
}

const babelwires::CommandManager<babelwires::Project>& babelwires::AccessModelScope::getCommandManager() {
    return m_projectGraphModel.m_commandManager;
}
