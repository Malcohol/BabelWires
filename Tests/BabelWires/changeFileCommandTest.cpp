#include <gtest/gtest.h>

#include "BabelWires/Commands/changeFileCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWires/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/TestUtils/tempFilePath.hpp"

namespace {
    void testSourceFileChange(bool source1Present, bool source2Present) {
        babelwires::FieldNameRegistryScope fieldNameRegistry;
        libTestUtils::TestProjectContext context;

        testUtils::TempFilePath filePath1("foo" + libTestUtils::TestSourceFileFormat::getFileExtension());
        testUtils::TempFilePath filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());

        if (source1Present) {
            libTestUtils::TestSourceFileFormat::writeToTestFile(filePath1, 'x');
        }
        if (source2Present) {
            libTestUtils::TestSourceFileFormat::writeToTestFile(filePath2, 'q');
        }

        babelwires::SourceFileData elementData;
        elementData.m_absoluteFilePath = filePath1;
        elementData.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();

        const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
        const auto* element =
            dynamic_cast<const babelwires::SourceFileElement*>(context.m_project.getFeatureElement(elementId));
        ASSERT_NE(element, nullptr);

        const auto getOutputFeature = [element]() {
            return dynamic_cast<const libTestUtils::TestFileFeature*>(element->getOutputFeature());
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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    std::string filePath1("foo" + libTestUtils::TestSourceFileFormat::getFileExtension());
    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileData elementData;
    elementData.m_absoluteFilePath = filePath1;
    elementData.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const auto* element =
        dynamic_cast<const babelwires::TargetFileElement*>(context.m_project.getFeatureElement(elementId));
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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", 57, filePath2);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

// Not sure how this could happen.
TEST(ChangeFileCommandTest, failSafelyNotAFileElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    std::string filePath2("erm" + libTestUtils::TestSourceFileFormat::getFileExtension());
    babelwires::ChangeFileCommand command("Test command", elementId, filePath2);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}
