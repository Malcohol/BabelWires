#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElement.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

namespace {
    bool endsWithStar(const std::string& label) { return label[label.length() - 1] == '*'; };
} // namespace

TEST(TargetFileElementTest, targetFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    std::ostringstream tempFileName;
    tempFileName << "foo." << testUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    auto featureElement = data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    babelwires::TargetFileElement* targetFileElement =
        static_cast<babelwires::TargetFileElement*>(featureElement.get());

    EXPECT_EQ(targetFileElement->getFilePath(), tempFilePath.m_filePath);
    EXPECT_EQ(targetFileElement->getSupportedFileOperations(), babelwires::FileElement::FileOperations::save);
    EXPECT_NE(targetFileElement->getFileFormatInformation(testEnvironment.m_projectContext), nullptr);
    EXPECT_EQ(targetFileElement->getFileFormatInformation(testEnvironment.m_projectContext)->getIdentifier(), testUtils::TestTargetFileFormat::getThisIdentifier());

    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();

    const babelwires::Path pathToInt = testUtils::getTestFileElementPathToInt0();

    {
        babelwires::ValueAssignmentData intMod(babelwires::IntValue(30));
        intMod.m_targetPath = pathToInt;
        targetFileElement->addModifier(testEnvironment.m_log, intMod);
    }

    targetFileElement->process(testEnvironment.m_project, testEnvironment.m_log);

    // Note: There's a small chance this will fail because hash collisions. If it does, change the intMod.m_value above.
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->removeModifier(targetFileElement->findModifier(pathToInt));
    targetFileElement->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));
}

TEST(TargetFileElementTest, changeFile) {
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName1;
    tempFileName1 << "foo1." << testUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath1(tempFileName1.str());

    std::ostringstream tempFileName2;
    tempFileName2 << "foo2." << testUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath2(tempFileName2.str());

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath1;

    auto featureElement = data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    babelwires::TargetFileElement* targetFileElement =
        static_cast<babelwires::TargetFileElement*>(featureElement.get());

    EXPECT_TRUE(targetFileElement->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();
    targetFileElement->setFilePath(tempFilePath2.m_filePath);
    targetFileElement->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_TRUE(targetFileElement->isChanged(babelwires::Node::Changes::FileChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::Node::Changes::FeatureElementLabelChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::Node::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();
    targetFileElement->setFilePath(tempFilePath1.m_filePath);
    targetFileElement->process(testEnvironment.m_project, testEnvironment.m_log);

    // setFilePath is not expected to trigger a reload.
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::Node::Changes::FileChanged));
    // The label already ends with a star.
    EXPECT_FALSE(targetFileElement->isChanged(babelwires::Node::Changes::FeatureElementLabelChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::Node::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
}
