#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

namespace {
    bool endsWithStar(const std::string& label) { return label[label.length() - 1] == '*'; };
} // namespace

TEST(TargetFileNodeTest, targetFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    std::ostringstream tempFileName;
    tempFileName << "foo." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());

    babelwires::TargetFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    auto node = data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    ASSERT_TRUE(node->as<babelwires::TargetFileNode>());
    babelwires::TargetFileNode* targetFileNode =
        static_cast<babelwires::TargetFileNode*>(node.get());

    EXPECT_EQ(targetFileNode->getFilePath(), tempFilePath.m_filePath);
    EXPECT_EQ(targetFileNode->getSupportedFileOperations(), babelwires::FileNode::FileOperations::save);
    EXPECT_NE(targetFileNode->getFileFormatInformation(testEnvironment.m_projectContext), nullptr);
    EXPECT_EQ(targetFileNode->getFileFormatInformation(testEnvironment.m_projectContext)->getIdentifier(), testDomain::TestTargetFileFormat::getThisIdentifier());

    EXPECT_TRUE(endsWithStar(targetFileNode->getLabel()));
    EXPECT_TRUE(targetFileNode->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileNode->getLabel()));

    targetFileNode->clearChanges();

    const babelwires::Path pathToInt = testDomain::getTestFileElementPathToInt0();

    {
        babelwires::ValueAssignmentData intMod(babelwires::IntValue(30));
        intMod.m_targetPath = pathToInt;
        targetFileNode->addModifier(testEnvironment.m_log, intMod);
    }

    targetFileNode->process(testEnvironment.m_project, testEnvironment.m_log);

    // Note: There's a small chance this will fail because hash collisions. If it does, change the intMod.m_value above.
    EXPECT_TRUE(endsWithStar(targetFileNode->getLabel()));
    EXPECT_TRUE(targetFileNode->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileNode->getLabel()));

    targetFileNode->removeModifier(targetFileNode->findModifier(pathToInt));
    targetFileNode->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_TRUE(endsWithStar(targetFileNode->getLabel()));
    EXPECT_TRUE(targetFileNode->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileNode->getLabel()));
}

TEST(TargetFileNodeTest, changeFile) {
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName1;
    tempFileName1 << "foo1." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath1(tempFileName1.str());

    std::ostringstream tempFileName2;
    tempFileName2 << "foo2." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath2(tempFileName2.str());

    babelwires::TargetFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath1;

    auto node = data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    ASSERT_TRUE(node->as<babelwires::TargetFileNode>());
    babelwires::TargetFileNode* targetFileNode =
        static_cast<babelwires::TargetFileNode*>(node.get());

    EXPECT_TRUE(targetFileNode->save(testEnvironment.m_projectContext, testEnvironment.m_log));
    EXPECT_FALSE(endsWithStar(targetFileNode->getLabel()));

    targetFileNode->clearChanges();
    targetFileNode->setFilePath(tempFilePath2.m_filePath);
    targetFileNode->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_TRUE(targetFileNode->isChanged(babelwires::Node::Changes::FileChanged));
    EXPECT_TRUE(targetFileNode->isChanged(babelwires::Node::Changes::NodeLabelChanged));
    EXPECT_TRUE(targetFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileNode->getLabel()));

    targetFileNode->clearChanges();
    targetFileNode->setFilePath(tempFilePath1.m_filePath);
    targetFileNode->process(testEnvironment.m_project, testEnvironment.m_log);

    // setFilePath is not expected to trigger a reload.
    EXPECT_TRUE(targetFileNode->isChanged(babelwires::Node::Changes::FileChanged));
    // The label already ends with a star.
    EXPECT_FALSE(targetFileNode->isChanged(babelwires::Node::Changes::NodeLabelChanged));
    EXPECT_TRUE(targetFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileNode->getLabel()));
}
