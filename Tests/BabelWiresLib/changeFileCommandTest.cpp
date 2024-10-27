#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/changeFileCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

namespace {
    void testSourceFileChange(bool source1Present, bool source2Present) {
        testUtils::TestEnvironment testEnvironment;

        testUtils::TempFilePath filePath1("foo" + testUtils::TestSourceFileFormat::getFileExtension());
        testUtils::TempFilePath filePath2("erm" + testUtils::TestSourceFileFormat::getFileExtension());

        if (source1Present) {
            testUtils::TestSourceFileFormat::writeToTestFile(filePath1, 'x');
        }
        if (source2Present) {
            testUtils::TestSourceFileFormat::writeToTestFile(filePath2, 'q');
        }

        babelwires::SourceFileElementData elementData;
        elementData.m_filePath = filePath1;
        elementData.m_factoryIdentifier = testUtils::TestSourceFileFormat::getThisIdentifier();

        const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
        const auto* element =
            testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::SourceFileElement>();
        ASSERT_NE(element, nullptr);

        const auto getOutput = [element]() {
            return testUtils::TestSimpleRecordType::ConstInstance(*element->getOutput()->getFeature(0));
        };

        EXPECT_EQ(element->getFilePath(), filePath1.m_filePath);
        if (source1Present) {
            EXPECT_EQ(getOutput().getintR0().get(), 'x');
        }

        babelwires::ChangeFileCommand command("Test command", elementId, filePath2.m_filePath);

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

    std::string filePath1("foo" + testUtils::TestSourceFileFormat::getFileExtension());
    std::string filePath2("erm" + testUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileElementData elementData;
    elementData.m_filePath = filePath1;
    elementData.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::TargetFileElement>();
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

    std::string filePath2("erm" + testUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", 57, filePath2);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

// Not sure how this could happen.
TEST(ChangeFileCommandTest, failSafelyNotAFileElement) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    std::string filePath2("erm" + testUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
