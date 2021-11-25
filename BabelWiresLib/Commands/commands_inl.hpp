
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
bool babelwires::SimpleCommand<COMMAND_TARGET>::initializeAndExecute(Project& project) {
    if (initialize(project)) {
        this->template execute(project);
        return true;
    }
    return false;
}

template <typename COMMAND_TARGET>
babelwires::CompoundCommand<COMMAND_TARGET>::CompoundCommand(std::string commandName)
    : Command<COMMAND_TARGET>(std::move(commandName)) {}

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::addSubCommand(std::unique_ptr<Command<COMMAND_TARGET>> subCommand) {
    m_subCommands.emplace_back(std::move(subCommand));
}

template <typename COMMAND_TARGET>
bool babelwires::CompoundCommand<COMMAND_TARGET>::initializeAndExecute(Project& project) {
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

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::execute(Project& project) const {
    for (auto&& subCommand : m_subCommands) {
        subCommand->execute(project);
    }
}

template <typename COMMAND_TARGET>
void babelwires::CompoundCommand<COMMAND_TARGET>::undo(Project& project) const {
    std::for_each(m_subCommands.rbegin(), m_subCommands.rend(),
                  [&project](const auto& subCommand) { subCommand->undo(project); });
}
