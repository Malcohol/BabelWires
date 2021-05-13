/**
 * Commands define undoable ways of mutating the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWires/Commands/commandTimestamp.hpp"
#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWires/Project/projectIds.hpp"

#include <chrono>
#include <memory>
#include <ostream>
#include <string>

namespace babelwires {

    class Project;
    struct ElementData;
    struct TargetFileData;
    struct ConnectionModifierData;
    struct ModifierData;

    /// Commands define undoable ways of mutating the project.
    class Command {
      public:
        /// CommandName should be a displayable name
        /// The timestamp is set to the current time.
        Command(std::string commandName);

        virtual ~Command();

        std::string getName() const;

        /// Query the system to populate the data in the command, and
        /// execute it.
        /// Returns false if the command cannot be initialized (because the project
        /// is not in the expected state).
        /// When false is returned, the system should be unaffected.
        /// See command for execute about how commands should achieve their effect.
        virtual bool initializeAndExecute(Project& project) = 0;

        /// Perform the command.
        /// Note: A command should not modify feature contents directly. Instead, it should add or remove modifiers to achieve that effect.
        virtual void execute(Project& project) const = 0;

        /// When the system is in the state just after the execution of the command,
        /// restore the system to the state just prior.
        virtual void undo(Project& project) const = 0;

        /// Should the subsequentCommand be subsumed into this one, to appear as one command?
        /// Subsumption may be attempted before and after the commands are executed, and
        /// the commands can opt in or out as appropriate. When thisIsAlreadyExecuted, the
        /// subsequentCommand will be executed independently, but won't get its own undo stack entry.
        /// On the other hand, when thisIsAlreadyExecuted is false, the subsequentCommand should be
        /// folded into this, so the effect of executing this will be the effect of executing both.
        /// The default implementation returns false.
        // TODO The subsumption mechanism is a symptom of the particular callbacks the UI fires:
        //  * The move which follows an add node.
        //  * The continuous stream of callbacks when a node is dragged or resized.
        //  * The multiple remove node callbacks when a selection of nodes is deleted.
        // If the UI could be adjusted so these cases are better handled, we could possibly do without
        // the subsumption code.
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const;

        /// Append the command or commands in the subsequentCommand to this CompoundCommand.
        /// The default implementation asserts.
        virtual void subsume(std::unique_ptr<Command> subsequentCommand);

        const CommandTimestamp& getTimestamp() const;
        void setTimestamp(const CommandTimestamp& timestamp);

      private:
        std::string m_commandName;

        /// A command carries a timestamp, which is initialized to the current
        /// time at construction, but may be updated.
        CommandTimestamp m_timestamp;
    };

    /// A simple command can be initialized without affecting the state of the system,
    /// so it has three virtual methods to override: initialize, execute and undo.
    class SimpleCommand : public Command {
      public:
        SimpleCommand(std::string commandName);

        /// Call initialize and then execute.
        virtual bool initializeAndExecute(Project& project) override final;

        /// Query the project to capture and store a description of the undo operation.
        /// Return false if the command cannot be initialized.
        virtual bool initialize(const Project& project) = 0;
    };

    /// A compound command is composed of subcommands.
    class CompoundCommand : public Command {
      public:
        CompoundCommand(std::string commandName);

        void addSubCommand(std::unique_ptr<Command> subCommand);

        virtual bool initializeAndExecute(Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        std::vector<std::unique_ptr<Command>> m_subCommands;
    };

} // namespace babelwires
