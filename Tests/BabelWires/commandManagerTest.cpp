#include <gtest/gtest.h>

#include "BabelWires/Commands/commandManager.hpp"
#include "BabelWires/Commands/commands.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"

namespace {
    struct TestCommand : babelwires::Command {
        TestCommand(std::string commandName, bool shouldApply, bool isSubsumable = false)
            : Command(std::move(commandName))
            , m_shouldApply(shouldApply)
            , m_isSubsumable(isSubsumable) {}

        bool initializeAndExecute(babelwires::Project& project) override {
            if (m_shouldApply) {
                m_eventTrace.emplace_back(Event::initializedAndExecuted);
            }
            return m_shouldApply;
        }

        void execute(babelwires::Project& project) const override { m_eventTrace.emplace_back(Event::executed); }

        void undo(babelwires::Project& project) const override { m_eventTrace.emplace_back(Event::undone); }

        bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override {
            const auto* newCommand = subsequentCommand.as<TestCommand>();
            return (newCommand && newCommand->m_isSubsumable);
        }

        void subsume(std::unique_ptr<Command> subsequentCommand) {
            m_subsumedCommands.emplace_back(std::move(subsequentCommand));
        }

        enum class Event { initializedAndExecuted, executed, undone };

        bool m_shouldApply;
        bool m_isSubsumable;
        mutable std::vector<Event> m_eventTrace;
        std::vector<std::unique_ptr<Command>> m_subsumedCommands;
    };
} // namespace

TEST(CommandManagerTest, undoRedoSinceCommand) {
    libTestUtils::TestProjectContext context;
    babelwires::CommandManager commandManager(context.m_project, context.m_log);

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    TestCommand* testCommand = new TestCommand("perform test command", true);
    {
        std::unique_ptr<babelwires::Command> testCommandPtr(testCommand);
        commandManager.executeAndStealCommand(testCommandPtr);
        // Expect stolen
        EXPECT_EQ(testCommandPtr, nullptr);
    }
    ASSERT_NE(testCommand, nullptr);
    ASSERT_EQ(testCommand->m_eventTrace.size(), 1);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::initializedAndExecuted);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform test command");

    commandManager.undo();
    EXPECT_EQ(testCommand->m_eventTrace.size(), 2);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform test command");

    commandManager.redo();
    EXPECT_EQ(testCommand->m_eventTrace.size(), 3);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::executed);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    // the project is saved
    commandManager.setCursor();

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    commandManager.undo();
    EXPECT_EQ(testCommand->m_eventTrace.size(), 4);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    commandManager.redo();
    EXPECT_EQ(testCommand->m_eventTrace.size(), 5);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::executed);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());
}

TEST(CommandManagerTest, undoRedoWithTwoCommands) {
    libTestUtils::TestProjectContext context;
    babelwires::CommandManager commandManager(context.m_project, context.m_log);

    TestCommand* testCommand = new TestCommand("perform first test command", true);
    {
        std::unique_ptr<babelwires::Command> testCommandPtr(testCommand);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    ASSERT_EQ(testCommand->m_eventTrace.size(), 1);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::initializedAndExecuted);

    TestCommand* testCommand2 = new TestCommand("perform second test command", true);
    {
        std::unique_ptr<babelwires::Command> testCommandPtr(testCommand2);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    ASSERT_EQ(testCommand2->m_eventTrace.size(), 1);
    EXPECT_EQ(testCommand2->m_eventTrace.back(), TestCommand::Event::initializedAndExecuted);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform second test command");

    commandManager.undo();
    ASSERT_EQ(testCommand2->m_eventTrace.size(), 2);
    EXPECT_EQ(testCommand2->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform first test command");
    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform second test command");

    commandManager.undo();
    ASSERT_EQ(testCommand->m_eventTrace.size(), 2);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform first test command");

    commandManager.redo();
    ASSERT_EQ(testCommand->m_eventTrace.size(), 3);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::executed);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform first test command");
    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform second test command");

    // the project is saved
    commandManager.setCursor();

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    commandManager.redo();
    ASSERT_EQ(testCommand2->m_eventTrace.size(), 3);
    EXPECT_EQ(testCommand2->m_eventTrace.back(), TestCommand::Event::executed);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform second test command");

    commandManager.undo();
    ASSERT_EQ(testCommand2->m_eventTrace.size(), 4);
    EXPECT_EQ(testCommand2->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform first test command");
    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform second test command");

    commandManager.undo();
    ASSERT_EQ(testCommand->m_eventTrace.size(), 4);
    EXPECT_EQ(testCommand->m_eventTrace.back(), TestCommand::Event::undone);

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform first test command");
}

TEST(CommandManagerTest, failedCommand) {
    libTestUtils::TestProjectContext context;
    babelwires::CommandManager commandManager(context.m_project, context.m_log);

    TestCommand* failedCommand = new TestCommand("attempt impossible command", false);
    {
        std::unique_ptr<babelwires::Command> failedCommandPtr(failedCommand);
        commandManager.executeAndStealCommand(failedCommandPtr);
        // The command did not get stolen.
        ASSERT_NE(failedCommandPtr, nullptr);
        // Did not get executed, etc.
        EXPECT_TRUE(failedCommand->m_eventTrace.empty());
    }

    context.m_log.hasSubstringIgnoreCase("Executing command \"attempt impossible command\"");
    context.m_log.hasSubstringIgnoreCase("Could not execute command \"attempt impossible command\"");

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    {
        std::unique_ptr<babelwires::Command> testCommandPtr =
            std::make_unique<TestCommand>("perform test command", true);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    commandManager.setCursor();
    {
        std::unique_ptr<babelwires::Command> testCommandPtr =
            std::make_unique<TestCommand>("perform another test command", true);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    commandManager.undo();
    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform test command");
    EXPECT_EQ(commandManager.getDescriptionOfRedoableCommand(), "perform another test command");

    TestCommand* failedCommand2 = new TestCommand("attempt another impossible command", false);
    {
        std::unique_ptr<babelwires::Command> failedCommandPtr(failedCommand2);
        commandManager.executeAndStealCommand(failedCommandPtr);
        // The command did not get stolen.
        ASSERT_NE(failedCommandPtr, nullptr);
        // Did not get executed, etc.
        EXPECT_TRUE(failedCommand2->m_eventTrace.empty());
    }

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform test command");
}

TEST(CommandManagerTest, subsumption) {
    libTestUtils::TestProjectContext context;
    babelwires::CommandManager commandManager(context.m_project, context.m_log);

    TestCommand* testCommand = new TestCommand("perform test command", true);
    {
        std::unique_ptr<babelwires::Command> testCommandPtr(testCommand);
        commandManager.executeAndStealCommand(testCommandPtr);
    }

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    TestCommand* subsumableCommand = new TestCommand("subsumable command", true, true);
    {
        std::unique_ptr<babelwires::Command> subsumableCommandPtr(subsumableCommand);
        commandManager.executeAndStealCommand(subsumableCommandPtr);
        // Expect stolen.
        EXPECT_EQ(subsumableCommandPtr, nullptr);
    }
    ASSERT_NE(subsumableCommand, nullptr);
    ASSERT_EQ(subsumableCommand->m_eventTrace.size(), 1);
    EXPECT_EQ(subsumableCommand->m_eventTrace.back(), TestCommand::Event::initializedAndExecuted);

    EXPECT_EQ(testCommand->m_subsumedCommands.size(), 1);

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    EXPECT_EQ(commandManager.getDescriptionOfUndoableCommand(), "perform test command");
}

TEST(CommandManagerTest, clear) {
    libTestUtils::TestProjectContext context;
    babelwires::CommandManager commandManager(context.m_project, context.m_log);

    {
        std::unique_ptr<babelwires::Command> testCommandPtr =
            std::make_unique<TestCommand>("perform first test command", true);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    {
        std::unique_ptr<babelwires::Command> testCommandPtr =
            std::make_unique<TestCommand>("perform second test command", true);
        commandManager.executeAndStealCommand(testCommandPtr);
    }
    commandManager.undo();

    EXPECT_TRUE(commandManager.canUndo());
    EXPECT_TRUE(commandManager.canRedo());
    EXPECT_FALSE(commandManager.isAtCursor());

    commandManager.clear();

    EXPECT_FALSE(commandManager.canUndo());
    EXPECT_FALSE(commandManager.canRedo());
    EXPECT_TRUE(commandManager.isAtCursor());
}
