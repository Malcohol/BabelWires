/**
 * The CommandManager manages undo and redo, and provides the functionality to determine
 * if a file has changed since it was last saved.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Commands/commandManager.hpp"
#include "BabelWiresLib/Commands/commands.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/projectData.hpp"

#include "Common/Log/debugLogger.hpp"
#include "Common/Log/userLogger.hpp"

#include <cassert>

babelwires::CommandManager::CommandManager(Project& project, UserLogger& userLogger)
    : m_project(project)
    , m_userLogger(userLogger) {}

babelwires::CommandManager::~CommandManager() = default;

bool babelwires::CommandManager::executeAndStealCommand(std::unique_ptr<Command>& command) {
    const bool shouldSubsume = hasLastCommand() && getLastCommand().shouldSubsume(*command, true);
    if (!shouldSubsume) {
        m_userLogger.logInfo() << "Executing command \"" << command->getName() << "\"";
    } else {
        // TODO Might be too spammy, even for debug log.
        logDebug() << "Executing subsumed command \"" << command->getName() << "\"";
    }

    if (!command->initializeAndExecute(m_project)) {
        m_userLogger.logError() << "Could not execute command \"" << command->getName()
                                << "\": the project must have changed";
        // Clear redo stack.
        assert(((int)m_commandHistory.size() > m_indexOfAppliedCommand) && "m_indexOfAppliedCommand out of range");
        m_commandHistory.resize(m_indexOfAppliedCommand + 1);
        // If the save-point is after the last command, then clear it.
        if (m_indexOfCursor > m_indexOfAppliedCommand) {
            m_indexOfCursor = -1;
        }
        signal_undoStateChanged.fire();
        return false;
    }

    if (shouldSubsume) {
        getLastCommand().subsume(std::move(command));
    } else {
        ++m_indexOfAppliedCommand;
        m_commandHistory.resize(m_indexOfAppliedCommand + 1);
        m_commandHistory[m_indexOfAppliedCommand] = std::move(command);

        if (m_indexOfCursor >= m_indexOfAppliedCommand) {
            m_indexOfCursor = -1;
        }
        signal_undoStateChanged.fire();
    }

    return true;
}

bool babelwires::CommandManager::canUndo() const {
    return 0 <= m_indexOfAppliedCommand;
}

bool babelwires::CommandManager::canRedo() const {
    return m_indexOfAppliedCommand + 1 < m_commandHistory.size();
}

void babelwires::CommandManager::undo() {
    assert(canUndo() && "Call should be preceeded by canUndo");
    auto& command = m_commandHistory.at(m_indexOfAppliedCommand);
    m_userLogger.logInfo() << "Undoing command \"" << command->getName() << "\"";
    command->undo(m_project);
    --m_indexOfAppliedCommand;
    signal_undoStateChanged.fire();
}

void babelwires::CommandManager::redo() {
    assert(canRedo() && "Call should be preceeded by canRedo");
    ++m_indexOfAppliedCommand;
    auto& command = m_commandHistory.at(m_indexOfAppliedCommand);
    m_userLogger.logInfo() << "Redoing command \"" << command->getName() << "\"";
    command->execute(m_project);
    signal_undoStateChanged.fire();
}

std::string babelwires::CommandManager::getDescriptionOfUndoableCommand() const {
    assert(canUndo() && "Call should be preceeded by canUndo");
    return m_commandHistory.at(m_indexOfAppliedCommand)->getName();
}

std::string babelwires::CommandManager::getDescriptionOfRedoableCommand() const {
    assert(canRedo() && "Call should be preceeded by canRedo");
    return m_commandHistory.at(m_indexOfAppliedCommand + 1)->getName();
}

void babelwires::CommandManager::clear() {
    m_commandHistory.clear();
    m_indexOfAppliedCommand = -1;
    m_indexOfCursor = -1;
    signal_undoStateChanged.fire();
}

void babelwires::CommandManager::setCursor() {
    m_indexOfCursor = m_indexOfAppliedCommand;
    signal_undoStateChanged.fire();
}

bool babelwires::CommandManager::isAtCursor() const {
    return m_indexOfCursor == m_indexOfAppliedCommand;
}

bool babelwires::CommandManager::hasLastCommand() const {
    return canUndo() && !canRedo();
}

babelwires::Command& babelwires::CommandManager::getLastCommand() {
    assert(hasLastCommand() && "No last command");
    return *m_commandHistory.back();
}

const babelwires::Command& babelwires::CommandManager::getLastCommand() const {
    assert(hasLastCommand() && "No last command");
    return *m_commandHistory.back();
}
