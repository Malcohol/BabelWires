#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/changeFileCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/TestUtils/tempFilePath.hpp"

namespace {
    void testSourceFileChange(bool source1Present, bool source2Present) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        testUtils::TempFilePath filePath1("foo" + libTestUtils::TestSourceFileFormat::getFileExtension());
        testUtils::TempFilePath filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());

        if (source1Present) {
            libTestUtils::TestSourceFileFormat::writeToTestFile(filePath1, 'x');
        }
        if (source2Present) {
            libTestUtils::TestSourceFileFormat::writeToTestFile(filePath2, 'q');
        }

        babelwires::SourceFileElementData elementData;
        elementData.m_filePath = filePath1;
        elementData.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();

        const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
        const auto* element =
            context.m_project.getFeatureElement(elementId)->as<babelwires::SourceFileElement>();
        ASSERT_NE(element, nullptr);

        const auto getOutputFeature = [element]() {
            return element->getOutputFeature()->as<const libTestUtils::TestFileFeature>();
        };

        EXPECT_EQ(element->getFilePath(), filePath1.m_filePath);
        if (source1Present) {
            ASSERT_NE(getOutputFeature(), nullptr);
            EXPECT_EQ(getOutputFeature()->m_intChildFeature->get(), 'x');
        }

        babelwires::ChangeFileCommand command("Test command", elementId, filePath2.m_filePath);

        EXPECT_EQ(command.getName(), "Test command");

        context.m_project.process();
        EXPECT_TRUE(command.initialize(context.m_project));
        command.execute(context.m_project);
        context.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath2.m_filePath);
        if (source2Present) {
            EXPECT_EQ(getOutputFeature()->m_intChildFeature->get(), 'q');
        }

        command.undo(context.m_project);
        context.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath1.m_filePath);
        if (source1Present) {
            EXPECT_EQ(getOutputFeature()->m_intChildFeature->get(), 'x');
        }

        command.execute(context.m_project);
        context.m_project.process();

        EXPECT_EQ(element->getFilePath(), filePath2.m_filePath);
        if (source2Present) {
            EXPECT_EQ(getOutputFeature()->m_intChildFeature->get(), 'q');
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
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    std::string filePath1("foo" + libTestUtils::TestSourceFileFormat::getFileExtension());
    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileElementData elementData;
    elementData.m_filePath = filePath1;
    elementData.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<babelwires::TargetFileElement>();
    ASSERT_NE(element, nullptr);

    EXPECT_EQ(element->getFilePath(), filePath1);

    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));
    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath2);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath1);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(element->getFilePath(), filePath2);
}

TEST(ChangeFileCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", 57, filePath2);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

// Not sure how this could happen.
TEST(ChangeFileCommandTest, failSafelyNotAFileElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}
