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

#include "Common/Log/debugLogger.hpp"
#include "Common/Log/userLogger.hpp"

#include <cassert>

template<typename COMMAND_TARGET> babelwires::CommandManager<COMMAND_TARGET>::CommandManager(COMMAND_TARGET& target, UserLogger& userLogger)
    : m_target(target)
    , m_userLogger(userLogger) {}

template<typename COMMAND_TARGET> babelwires::CommandManager<COMMAND_TARGET>::~CommandManager() = default;

template<typename COMMAND_TARGET> bool babelwires::CommandManager<COMMAND_TARGET>::executeAndStealCommand(std::unique_ptr<Command<COMMAND_TARGET>>& command) {
    const bool shouldSubsume = hasLastCommand() && getLastCommand().shouldSubsume(*command, true);
    if (!shouldSubsume) {
        m_userLogger.logInfo() << "Executing command \"" << command->getName() << "\"";
    } else {
        // TODO Might be too spammy, even for debug log.
        logDebug() << "Executing subsumed command \"" << command->getName() << "\"";
    }

    if (!command->initializeAndExecute(m_target)) {
        m_userLogger.logError() << "Could not execute command \"" << command->getName()
                                << "\": the system must have changed";
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

template<typename COMMAND_TARGET> bool babelwires::CommandManager<COMMAND_TARGET>::canUndo() const {
    return 0 <= m_indexOfAppliedCommand;
}

template<typename COMMAND_TARGET> bool babelwires::CommandManager<COMMAND_TARGET>::canRedo() const {
    return m_indexOfAppliedCommand + 1 < m_commandHistory.size();
}

template<typename COMMAND_TARGET> void babelwires::CommandManager<COMMAND_TARGET>::undo() {
    assert(canUndo() && "Call should be preceeded by canUndo");
    auto& command = m_commandHistory.at(m_indexOfAppliedCommand);
    m_userLogger.logInfo() << "Undoing command \"" << command->getName() << "\"";
    command->undo(m_target);
    --m_indexOfAppliedCommand;
    signal_undoStateChanged.fire();
}

template<typename COMMAND_TARGET> void babelwires::CommandManager<COMMAND_TARGET>::redo() {
    assert(canRedo() && "Call should be preceeded by canRedo");
    ++m_indexOfAppliedCommand;
    auto& command = m_commandHistory.at(m_indexOfAppliedCommand);
    m_userLogger.logInfo() << "Redoing command \"" << command->getName() << "\"";
    command->execute(m_target);
    signal_undoStateChanged.fire();
}

template<typename COMMAND_TARGET> std::string babelwires::CommandManager<COMMAND_TARGET>::getDescriptionOfUndoableCommand() const {
    assert(canUndo() && "Call should be preceeded by canUndo");
    return m_commandHistory.at(m_indexOfAppliedCommand)->getName();
}

template<typename COMMAND_TARGET> std::string babelwires::CommandManager<COMMAND_TARGET>::getDescriptionOfRedoableCommand() const {
    assert(canRedo() && "Call should be preceeded by canRedo");
    return m_commandHistory.at(m_indexOfAppliedCommand + 1)->getName();
}

template<typename COMMAND_TARGET> void babelwires::CommandManager<COMMAND_TARGET>::clear() {
    m_commandHistory.clear();
    m_indexOfAppliedCommand = -1;
    m_indexOfCursor = -1;
    signal_undoStateChanged.fire();
}

template<typename COMMAND_TARGET> void babelwires::CommandManager<COMMAND_TARGET>::setCursor() {
    m_indexOfCursor = m_indexOfAppliedCommand;
    signal_undoStateChanged.fire();
}

template<typename COMMAND_TARGET> bool babelwires::CommandManager<COMMAND_TARGET>::isAtCursor() const {
    return m_indexOfCursor == m_indexOfAppliedCommand;
}

template<typename COMMAND_TARGET> bool babelwires::CommandManager<COMMAND_TARGET>::hasLastCommand() const {
    return canUndo() && !canRedo();
}

template<typename COMMAND_TARGET> babelwires::Command<COMMAND_TARGET>& babelwires::CommandManager<COMMAND_TARGET>::getLastCommand() {
    assert(hasLastCommand() && "No last command");
    return *m_commandHistory.back();
}

template<typename COMMAND_TARGET> const babelwires::Command<COMMAND_TARGET>& babelwires::CommandManager<COMMAND_TARGET>::getLastCommand() const {
    assert(hasLastCommand() && "No last command");
    return *m_commandHistory.back();
}
