/**
 * Commands define undoable ways of mutating the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWires/Commands/commands.hpp"

#include "BabelWires/Project/project.hpp"

#include <algorithm>
#include <cassert>

babelwires::Command::Command(std::string commandName)
    : m_commandName(std::move(commandName))
    , m_timestamp(std::chrono::steady_clock::now()) {
    assert(!m_commandName.empty() && "You must provide a name for the command");
}

babelwires::Command::~Command() {}

std::string babelwires::Command::getName() const {
    return m_commandName;
}

const babelwires::CommandTimestamp& babelwires::Command::getTimestamp() const {
    return m_timestamp;
}

void babelwires::Command::setTimestamp(const CommandTimestamp& timestamp) {
    m_timestamp = timestamp;
}

bool babelwires::Command::shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const {
    return false;
}

void babelwires::Command::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(!"A overriding implementation should be called");
}

babelwires::SimpleCommand::SimpleCommand(std::string commandName)
    : Command(std::move(commandName)) {}

bool babelwires::SimpleCommand::initializeAndExecute(Project& project) {
    if (initialize(project)) {
        execute(project);
        return true;
    }
    return false;
}

babelwires::CompoundCommand::CompoundCommand(std::string commandName)
    : Command(std::move(commandName)) {}

void babelwires::CompoundCommand::addSubCommand(std::unique_ptr<Command> subCommand) {
    m_subCommands.emplace_back(std::move(subCommand));
}

bool babelwires::CompoundCommand::initializeAndExecute(Project& project) {
    int i = 0;
    for (; i < m_subCommands.size(); ++i) {
        if (!m_subCommands[i]->initializeAndExecute(project)) {
            break;
        }
    }
    if (i < m_subCommands.size()) {
        // Not all subcommands succeeded, so restore the initial state.
        for (--i; i >= 0; --i) {
            m_subCommands[i]->undo(project);
        }
        return false;
    }
    return true;
}

void babelwires::CompoundCommand::execute(Project& project) const {
    for (auto&& subCommand : m_subCommands) {
        subCommand->execute(project);
    }
}

void babelwires::CompoundCommand::undo(Project& project) const {
    std::for_each(m_subCommands.rbegin(), m_subCommands.rend(),
                  [&project](const auto& subCommand) { subCommand->undo(project); });
}
