#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

namespace {
    void createTestFile(testUtils::TestEnvironment& testEnvironment, const std::filesystem::path& path,
                        int value = 14) {
        auto fileFormat = std::make_unique<testDomain::TestTargetFileFormat>();
        auto fileFeature = std::make_unique<babelwires::ValueTreeRoot>(testEnvironment.m_projectContext.m_typeSystem,
                                                                       testDomain::getTestFileType());
        fileFeature->setToDefault();
        testDomain::TestSimpleRecordType::Instance instance{fileFeature->getChild(0)->is<babelwires::ValueTreeNode>()};
        instance.getintR0().set(value);
        fileFormat->writeToFile(testEnvironment.m_projectContext, testEnvironment.m_log, *fileFeature, path);
    }
} // namespace

TEST(SourceFileNodeTest, sourceFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName;
    tempFileName << "foo." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());

    createTestFile(testEnvironment, tempFilePath);

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    auto node = data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    ASSERT_TRUE(node->as<babelwires::SourceFileNode>());
    babelwires::SourceFileNode* sourceFileNode = static_cast<babelwires::SourceFileNode*>(node.get());

    EXPECT_EQ(sourceFileNode->getFilePath(), tempFilePath.m_filePath);
    EXPECT_EQ(sourceFileNode->getSupportedFileOperations(), babelwires::FileNode::FileOperations::reload);
    EXPECT_NE(sourceFileNode->getFileFormatInformation(testEnvironment.m_projectContext), nullptr);
    EXPECT_EQ(sourceFileNode->getFileFormatInformation(testEnvironment.m_projectContext)->getIdentifier(),
              testDomain::TestSourceFileFormat::getThisIdentifier());

    std::filesystem::remove(tempFilePath);

    sourceFileNode->clearChanges();
    testEnvironment.m_log.clear();
    EXPECT_FALSE(sourceFileNode->reload(testEnvironment.m_projectContext, testEnvironment.m_log));

    EXPECT_TRUE(sourceFileNode->isFailed());
    std::ostringstream pathInError;
    pathInError << tempFilePath.m_filePath;
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase(pathInError.str()));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("could not be loaded"));
    EXPECT_TRUE(sourceFileNode->getReasonForFailure().find(pathInError.str()) != std::string::npos);

    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::NodeFailed));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::FeatureStructureChanged));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));

    createTestFile(testEnvironment, tempFilePath);

    sourceFileNode->clearChanges();
    testEnvironment.m_log.clear();
    EXPECT_TRUE(sourceFileNode->reload(testEnvironment.m_projectContext, testEnvironment.m_log));

    EXPECT_FALSE(sourceFileNode->isFailed());
    EXPECT_TRUE(sourceFileNode->getReasonForFailure().empty());

    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::NodeRecovered));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::FeatureStructureChanged));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));
}

TEST(SourceFileNodeTest, changeFile) {
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName1;
    tempFileName1 << "foo1." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath1(tempFileName1.str());

    std::ostringstream tempFileName2;
    tempFileName2 << "foo2." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath2(tempFileName2.str());

    createTestFile(testEnvironment, tempFilePath1, 18);
    createTestFile(testEnvironment, tempFilePath2, 24);

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath1;

    auto node = data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    ASSERT_TRUE(node->as<babelwires::SourceFileNode>());
    babelwires::SourceFileNode* sourceFileNode = static_cast<babelwires::SourceFileNode*>(node.get());

    sourceFileNode->clearChanges();
    sourceFileNode->setFilePath(tempFilePath2.m_filePath);

    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::FileChanged));
    // setFilePath is not expected to trigger a reload.
    EXPECT_FALSE(sourceFileNode->isChanged(babelwires::Node::Changes::FeatureChangesMask));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));

    sourceFileNode->clearChanges();
    sourceFileNode->setFilePath(tempFilePath1.m_filePath);

    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::FileChanged));
    // setFilePath is not expected to trigger a reload.
    EXPECT_FALSE(sourceFileNode->isChanged(babelwires::Node::Changes::FeatureChangesMask));
    EXPECT_TRUE(sourceFileNode->isChanged(babelwires::Node::Changes::SomethingChanged));
}
