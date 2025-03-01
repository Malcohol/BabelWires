/**
 * Commands define undoable ways of mutating the a COMMAND_TARGET.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

template <typename COMMAND_TARGET>
babelwires::Command<COMMAND_TARGET>::Command(std::string commandName)
    : m_commandName(std::move(commandName))
    , m_timestamp(std::chrono::steady_clock::now()) {
    assert(!m_commandName.empty() && "You must provide a name for the command");
}

template <typename COMMAND_TARGET> babelwires::Command<COMMAND_TARGET>::~Command() {}

template <typename COMMAND_TARGET> std::string babelwires::Command<COMMAND_TARGET>::getName() const {
    return m_commandName;
}

template <typename COMMAND_TARGET>
const babelwires::CommandTimestamp& babelwires::Command<COMMAND_TARGET>::getTimestamp() const {
    return m_timestamp;
}

template <typename COMMAND_TARGET>
void babelwires::Command<COMMAND_TARGET>::setTimestamp(const CommandTimestamp& timestamp) {
    m_timestamp = timestamp;
}

template <typename COMMAND_TARGET>
bool babelwires::Command<COMMAND_TARGET>::shouldSubsume(const Command& subsequentCommand,
                                                        bool thisIsAlreadyExecuted) const {
    return false;
}

template <typename COMMAND_TARGET>
void babelwires::Command<COMMAND_TARGET>::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(!"A overriding implementation should be called");
}

template <typename COMMAND_TARGET>
babelwires::SimpleCommand<COMMAND_TARGET>::SimpleCommand(std::string commandName)
    : Command<COMMAND_TARGET>(std::move(commandName)) {}

template <typename COMMAND_TARGET>
bool babelwires::SimpleCommand<COMMAND_TARGET>::initializeAndExecute(COMMAND_TARGET& target) {
    if (initialize(target)) {
        this->execute(target);
        return true;
    }
    return false;
}

template <typename COMMAND_TARGET>
babelwires::CompoundCommand<COMMAND_TARGET>::CompoundCommand(std::string commandName)
    : Command<COMMAND_TARGET>(std::move(commandName)) {}

template <typename COMMAND_TARGET>
babelwires::CompoundCommand<COMMAND_TARGET>::CompoundCommand(const CompoundCommand& other)
    : Command<COMMAND_TARGET>(other) {
    m_subCommands.reserve(other.m_subCommands.size());
    for (const auto& c : other.m_subCommands) {
        m_subCommands.emplace_back(c->clone());
    };
}

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::addSubCommand(std::unique_ptr<Command<COMMAND_TARGET>> subCommand) {
    m_subCommands.emplace_back(std::move(subCommand));
}

template <typename COMMAND_TARGET>
bool babelwires::CompoundCommand<COMMAND_TARGET>::initializeAndExecute(COMMAND_TARGET& target) {
    int i = 0;
    for (; i < m_subCommands.size(); ++i) {
        if (!m_subCommands[i]->initializeAndExecute(target)) {
            break;
        }
    }
    if (i < m_subCommands.size()) {
        // Not all subcommands succeeded, so restore the initial state.
        for (--i; i >= 0; --i) {
            m_subCommands[i]->undo(target);
        }
        return false;
    }
    return true;
}

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::execute(COMMAND_TARGET& target) const {
    for (auto&& subCommand : m_subCommands) {
        subCommand->execute(target);
    }
}

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::undo(COMMAND_TARGET& target) const {
    std::for_each(m_subCommands.rbegin(), m_subCommands.rend(),
                  [&target](const auto& subCommand) { subCommand->undo(target); });
}
