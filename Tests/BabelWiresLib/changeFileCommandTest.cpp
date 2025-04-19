#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testNode.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

namespace {
    void testSourceFileChange(bool source1Present, bool source2Present) {
        testUtils::TestEnvironment testEnvironment;

        testUtils::TempFilePath filePath1("foo" + testDomain::TestSourceFileFormat::getFileExtension());
        testUtils::TempFilePath filePath2("erm" + testDomain::TestSourceFileFormat::getFileExtension());

        if (source1Present) {
            testDomain::TestSourceFileFormat::writeToTestFile(filePath1, 'x');
        }
        if (source2Present) {
            testDomain::TestSourceFileFormat::writeToTestFile(filePath2, 'q');
        }

        babelwires::SourceFileNodeData elementData;
        elementData.m_filePath = filePath1;
        elementData.m_factoryIdentifier = testDomain::TestSourceFileFormat::getThisIdentifier();

        const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
        const auto* element =
            testEnvironment.m_project.getNode(elementId)->as<babelwires::SourceFileNode>();
        ASSERT_NE(element, nullptr);

        const auto getOutput = [element]() {
            return testDomain::TestSimpleRecordType::ConstInstance(*element->getOutput()->getChild(0));
        };

        EXPECT_EQ(element->getFilePath(), filePath1.m_filePath);
        if (source1Present) {
            EXPECT_EQ(getOutput().getintR0().get(), 'x');
        }

        babelwires::ChangeFileCommand testCopyConstructor("Test command", elementId, filePath2.m_filePath);
        babelwires::ChangeFileCommand command = testCopyConstructor;

        EXPECT_EQ(command.getName(), "Test command");

        testEnvironment.m_project.process();
        EXPECT_TRUE(command.initialize(testEnvironment.m_project));
        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath2.m_filePath);
        if (source2Present) {
            EXPECT_EQ(getOutput().getintR0().get(), 'q');
        }

        command.undo(testEnvironment.m_project);
        testEnvironment.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath1.m_filePath);
        if (source1Present) {
            EXPECT_EQ(getOutput().getintR0().get(), 'x');
        }

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath2.m_filePath);
        if (source2Present) {
            EXPECT_EQ(getOutput().getintR0().get(), 'q');
        }
    }
} // namespace

TEST(ChangeFileCommandTest, executeAndUndoSourceBothPresent) {
    testSourceFileChange(true, true);
}

TEST(ChangeFileCommandTest, executeAndUndoSourceMissing1) {
    testSourceFileChange(false, true);
}

TEST(ChangeFileCommandTest, executeAndUndoSourceMissing2) {
    testSourceFileChange(true, false);
}

TEST(ChangeFileCommandTest, executeAndUndoSourceMissing1and2) {
    testSourceFileChange(false, false);
}

TEST(ChangeFileCommandTest, executeAndUndoTarget) {
    testUtils::TestEnvironment testEnvironment;

    std::string filePath1("foo" + testDomain::TestSourceFileFormat::getFileExtension());
    std::string filePath2("erm" + testDomain::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileNodeData elementData;
    elementData.m_filePath = filePath1;
    elementData.m_factoryIdentifier = testDomain::TestTargetFileFormat::getThisIdentifier();

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const auto* element = testEnvironment.m_project.getNode(elementId)->as<babelwires::TargetFileNode>();
    ASSERT_NE(element, nullptr);

    EXPECT_EQ(element->getFilePath(), filePath1);

    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath2);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath1);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath2);
}

TEST(ChangeFileCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    std::string filePath2("erm" + testDomain::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", 57, filePath2);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

// Not sure how this could happen.
TEST(ChangeFileCommandTest, failSafelyNotAFileElement) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestNodeData());

    std::string filePath2("erm" + testDomain::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
