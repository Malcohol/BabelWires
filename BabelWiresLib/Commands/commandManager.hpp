/**
 * The CommandManager manages undo and redo, and provides the functionality to determine
 * if a file has changed since it was last saved.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Signal/signal.hpp"

#include <memory>
#include <vector>

namespace babelwires {
    struct UserLogger;
}

namespace babelwires {

    template <typename COMMAND_TARGET> class Command;

    /// Manages commands for undo/redo, and has a "cursor" which can be used
    /// to know when the project has changed from its last saved state.
    template <typename COMMAND_TARGET> class CommandManager {
      public:
        CommandManager(COMMAND_TARGET& project, UserLogger& userLogger);
        virtual ~CommandManager();

        /// Returns true if the command could be executed, in which case ownership is moved to
        /// the command manager. If the command could not be executed, then ownership is left
        /// with the caller.
        bool executeAndStealCommand(std::unique_ptr<Command<COMMAND_TARGET>>& command);

        bool canUndo() const;
        bool canRedo() const;
        void undo();
        void redo();
        std::string getDescriptionOfUndoableCommand() const;
        std::string getDescriptionOfRedoableCommand() const;

        /// Discard the command history.
        void clear();

        /// Set the cursor to the current position.
        void setCursor();

        /// Is the project still in the state it was when the cursor was last set.
        bool isAtCursor() const;

      protected:
        /// Was an operation just performed?
        bool hasLastCommand() const;

        /// Get the last command performed.
        /// This is not allowed after an undo.
        Command<COMMAND_TARGET>& getLastCommand();
        const Command<COMMAND_TARGET>& getLastCommand() const;

      public: // Signals
        Signal<> signal_undoStateChanged;

      private:
        COMMAND_TARGET& m_project;
        UserLogger& m_userLogger;
        std::vector<std::unique_ptr<Command<COMMAND_TARGET>>> m_commandHistory;
        int m_indexOfAppliedCommand = -1;
        // The index in the command history corresponding to command performed when the project was last saved, or -1.
        int m_indexOfCursor = -1;
    };

} // namespace babelwires

#include <BabelWiresLib/Commands/commandManager_inl.hpp>