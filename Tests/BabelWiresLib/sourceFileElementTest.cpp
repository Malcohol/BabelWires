#include <gtest/gtest.h>

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

#include <fstream>

namespace {
    void createTestFile(libTestUtils::TestProjectContext& context, const std::filesystem::path& path, int value = 14) {
        std::ofstream tempFile(path);

        auto fileFormat = std::make_unique<libTestUtils::TestTargetFileFormat>();
        auto fileFeature = std::make_unique<libTestUtils::TestFileFeature>();
        fileFeature->m_intChildFeature->set(value);
        fileFormat->writeToFile(*fileFeature, tempFile, context.m_log);
    }
} // namespace

TEST(SourceFileElementTest, sourceFileDataCreateElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;

    // Create a test file.
    std::ostringstream tempFileName;
    tempFileName << "foo." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());

    createTestFile(context, tempFilePath);

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    auto featureElement = data.createFeatureElement(context.m_projectContext, context.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::SourceFileElement>());
    babelwires::SourceFileElement* sourceFileElement =
        static_cast<babelwires::SourceFileElement*>(featureElement.get());

    EXPECT_EQ(sourceFileElement->getFilePath(), tempFilePath.m_filePath);
    EXPECT_EQ(sourceFileElement->getSupportedFileOperations(), babelwires::FileElement::FileOperations::reload);
    EXPECT_NE(sourceFileElement->getFileFormatInformation(context.m_projectContext), nullptr);
    EXPECT_EQ(sourceFileElement->getFileFormatInformation(context.m_projectContext)->getIdentifier(), libTestUtils::TestSourceFileFormat::getThisIdentifier());

    std::filesystem::remove(tempFilePath);

    sourceFileElement->clearChanges();
    context.m_log.clear();
    EXPECT_FALSE(sourceFileElement->reload(context.m_projectContext, context.m_log));

    EXPECT_TRUE(sourceFileElement->isFailed());
    std::ostringstream pathInError;
    pathInError << tempFilePath.m_filePath;
    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase(pathInError.str()));
    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase("could not be loaded"));
    EXPECT_TRUE(sourceFileElement->getReasonForFailure().find(pathInError.str()) != std::string::npos);

    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    createTestFile(context, tempFilePath);

    sourceFileElement->clearChanges();
    context.m_log.clear();
    EXPECT_TRUE(sourceFileElement->reload(context.m_projectContext, context.m_log));

    EXPECT_FALSE(sourceFileElement->isFailed());
    EXPECT_TRUE(sourceFileElement->getReasonForFailure().empty());

    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST(SourceFileElementTest, changeFile) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;

    // Create a test file.
    std::ostringstream tempFileName1;
    tempFileName1 << "foo1." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath1(tempFileName1.str());

    std::ostringstream tempFileName2;
    tempFileName2 << "foo2." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath2(tempFileName2.str());

    createTestFile(context, tempFilePath1, 18);
    createTestFile(context, tempFilePath2, 24);

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath1;

    auto featureElement = data.createFeatureElement(context.m_projectContext, context.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::SourceFileElement>());
    babelwires::SourceFileElement* sourceFileElement =
        static_cast<babelwires::SourceFileElement*>(featureElement.get());

    sourceFileElement->clearChanges();
    sourceFileElement->setFilePath(tempFilePath2.m_filePath);

    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FileChanged));
    // setFilePath is not expected to trigger a reload.
    EXPECT_FALSE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    sourceFileElement->clearChanges();
    sourceFileElement->setFilePath(tempFilePath1.m_filePath);

    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FileChanged));
    // setFilePath is not expected to trigger a reload.
    EXPECT_FALSE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(sourceFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}
