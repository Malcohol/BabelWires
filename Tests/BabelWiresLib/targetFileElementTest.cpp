#include <gtest/gtest.h>

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

#include <fstream>

namespace {
    bool endsWithStar(const std::string& label) { return label[label.length() - 1] == '*'; };
} // namespace

TEST(TargetFileElementTest, targetFileDataCreateElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;

    std::ostringstream tempFileName;
    tempFileName << "foo." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    auto featureElement = data.createFeatureElement(context.m_projectContext, context.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    babelwires::TargetFileElement* targetFileElement =
        static_cast<babelwires::TargetFileElement*>(featureElement.get());

    EXPECT_EQ(targetFileElement->getFilePath(), tempFilePath.m_filePath);
    EXPECT_EQ(targetFileElement->getSupportedFileOperations(), babelwires::FileElement::FileOperations::save);
    EXPECT_NE(targetFileElement->getFileFormatInformation(context.m_projectContext), nullptr);
    EXPECT_EQ(targetFileElement->getFileFormatInformation(context.m_projectContext)->getIdentifier(), libTestUtils::TestTargetFileFormat::getThisIdentifier());

    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(context.m_projectContext, context.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();

    const babelwires::FeaturePath pathToInt =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestFileFeature::s_intChildInitializer);

    {
        babelwires::IntValueAssignmentData intMod;
        intMod.m_pathToFeature = pathToInt;
        intMod.m_value = 30;
        targetFileElement->addModifier(context.m_log, intMod);
    }

    targetFileElement->process(context.m_log);

    // Note: There's a small chance this will fail because hash collisions. If it does, change the intMod.m_value above.
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(context.m_projectContext, context.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->removeModifier(targetFileElement->findModifier(pathToInt));
    targetFileElement->process(context.m_log);

    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
    EXPECT_TRUE(targetFileElement->save(context.m_projectContext, context.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));
}

TEST(TargetFileElementTest, changeFile) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;

    // Create a test file.
    std::ostringstream tempFileName1;
    tempFileName1 << "foo1." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath1(tempFileName1.str());

    std::ostringstream tempFileName2;
    tempFileName2 << "foo2." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath2(tempFileName2.str());

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath1;

    auto featureElement = data.createFeatureElement(context.m_projectContext, context.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    babelwires::TargetFileElement* targetFileElement =
        static_cast<babelwires::TargetFileElement*>(featureElement.get());

    EXPECT_TRUE(targetFileElement->save(context.m_projectContext, context.m_log));
    EXPECT_FALSE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();
    targetFileElement->setFilePath(tempFilePath2.m_filePath);
    targetFileElement->process(context.m_log);

    EXPECT_TRUE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::FileChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementLabelChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));

    targetFileElement->clearChanges();
    targetFileElement->setFilePath(tempFilePath1.m_filePath);
    targetFileElement->process(context.m_log);

    // setFilePath is not expected to trigger a reload.
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::FileChanged));
    // The label already ends with a star.
    EXPECT_FALSE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementLabelChanged));
    EXPECT_TRUE(targetFileElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    EXPECT_TRUE(endsWithStar(targetFileElement->getLabel()));
}
