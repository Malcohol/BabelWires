#include <gtest/gtest.h>

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

#include "Common/Serialization/XML/xmlDeserializer.hpp"
#include "Common/Serialization/XML/xmlSerializer.hpp"

#include <fstream>

namespace {
    void setCommonFields(babelwires::ElementData& data) {
        data.m_id = 100;
        data.m_uiData.m_uiPosition.m_x = 12;
        data.m_uiData.m_uiPosition.m_y = -44;
        data.m_uiData.m_uiSize.m_width = 300;
        data.m_expandedPaths.emplace_back(babelwires::FeaturePath::deserializeFromString("aa/bb"));
    }

    void checkCommonFields(const babelwires::ElementData& data, bool testExpandedPaths = true) {
        EXPECT_EQ(data.m_id, 100);
        EXPECT_EQ(data.m_uiData.m_uiPosition.m_x, 12);
        EXPECT_EQ(data.m_uiData.m_uiPosition.m_y, -44);
        EXPECT_EQ(data.m_uiData.m_uiSize.m_width, 300);
        if (testExpandedPaths) {
            ASSERT_EQ(data.m_expandedPaths.size(), 1);
            EXPECT_EQ(data.m_expandedPaths[0], babelwires::FeaturePath::deserializeFromString("aa/bb"));
        } else {
            EXPECT_EQ(data.m_expandedPaths.size(), 0);
        }
    }

    void setModifiers(babelwires::ElementData& data, const char* pathStep) {
        auto newMod = std::make_unique<babelwires::IntValueAssignmentData>();
        newMod->m_pathToFeature = babelwires::FeaturePath::deserializeFromString(pathStep);
        newMod->m_value = 12;
        data.m_modifiers.emplace_back(std::move(newMod));
    }

    void checkModifiers(const babelwires::ElementData& data, const char* pathStep) {
        EXPECT_EQ(data.m_modifiers.size(), 1);
        EXPECT_NE(data.m_modifiers[0]->as<babelwires::IntValueAssignmentData>(), nullptr);
        const auto& mod = static_cast<const babelwires::IntValueAssignmentData&>(*data.m_modifiers[0]);
        EXPECT_EQ(mod.m_pathToFeature, babelwires::FeaturePath::deserializeFromString(pathStep));
        EXPECT_EQ(mod.m_value, 12);
    }
} // namespace

TEST(ElementDataTest, sourceFileDataClone) {
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    EXPECT_EQ(clone->m_filePath, std::filesystem::path("/a/b/c/foo.bar"));
}

TEST(ElementDataTest, sourceFileDataCustomClone) {
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_EQ(clone->m_filePath, std::filesystem::path("/a/b/c/foo.bar"));
}

TEST(ElementDataTest, sourceFileDataSerialize) {
    std::string serializedContents;
    {
        babelwires::SourceFileElementData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        data.m_filePath = "/a/b/c/foo.bar";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::SourceFileElementData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    EXPECT_EQ(dataPtr->m_filePath, std::filesystem::path("/a/b/c/foo.bar"));
}

TEST(ElementDataTest, sourceFileDataCreateElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    // Create a test file.
    std::ostringstream tempFileName;
    tempFileName << "foo." << libTestUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());
    {
        std::ofstream tempFile(tempFilePath);

        auto targetFileFormat = std::make_unique<libTestUtils::TestTargetFileFormat>();
        auto fileFeature = std::make_unique<libTestUtils::TestFileFeature>();
        fileFeature->m_intChildFeature->set(14);
        targetFileFormat->writeToFile(*fileFeature, tempFile, context.m_log);
    }

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::SourceFileElement>());
    EXPECT_TRUE(featureElement->getOutputFeature());
    EXPECT_TRUE(featureElement->getOutputFeature()->as<const libTestUtils::TestFileFeature>());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::SourceFileElementData>());
    EXPECT_EQ(static_cast<const babelwires::SourceFileElementData&>(featureElement->getElementData()).m_filePath,
              data.m_filePath);

    const libTestUtils::TestFileFeature* inputFeature =
        static_cast<const libTestUtils::TestFileFeature*>(featureElement->getOutputFeature());

    EXPECT_EQ(inputFeature->getFileFormatIdentifier(), libTestUtils::TestSourceFileFormat::getThisIdentifier());
    EXPECT_EQ(inputFeature->m_intChildFeature->get(), 14);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, targetFileDataClone) {
    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, libTestUtils::TestFileFeature::s_intChildInitializer);
    EXPECT_EQ(clone->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCustomClone) {
    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_TRUE(clone->m_modifiers.empty());
    EXPECT_EQ(clone->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataSerialize) {
    std::string serializedContents;
    {
        babelwires::TargetFileElementData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
        data.m_filePath = "/a/b/c/foo.bar";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::TargetFileElementData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, libTestUtils::TestFileFeature::s_intChildInitializer);
    EXPECT_EQ(dataPtr->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCreateElement) {
    libTestUtils::TestProjectContext context;

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = "foo";
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    babelwires::IdentifierRegistryScope identifierRegistry;

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_TRUE(featureElement->getInputFeature()->as<const libTestUtils::TestFileFeature>());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::TargetFileElementData>());
    EXPECT_EQ(static_cast<const babelwires::TargetFileElementData&>(featureElement->getElementData()).m_filePath,
              data.m_filePath);

    const libTestUtils::TestFileFeature* inputFeature =
        static_cast<const libTestUtils::TestFileFeature*>(featureElement->getInputFeature());

    EXPECT_EQ(inputFeature->getFileFormatIdentifier(), libTestUtils::TestSourceFileFormat::getThisIdentifier());
    EXPECT_EQ(inputFeature->m_intChildFeature->get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, processorDataClone) {
    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestRecordFeature::s_intIdInitializer);
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, libTestUtils::TestRecordFeature::s_intIdInitializer);
}

TEST(ElementDataTest, processorDataCustomClone) {
    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestRecordFeature::s_intIdInitializer);
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_TRUE(clone->m_modifiers.empty());
}

TEST(ElementDataTest, processorDataSerialize) {
    std::string serializedContents;
    {
        babelwires::ProcessorElementData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        setModifiers(data, libTestUtils::TestRecordFeature::s_intIdInitializer);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ProcessorElementData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, libTestUtils::TestRecordFeature::s_intIdInitializer);
}

TEST(ElementDataTest, processorDataCreateElement) {
    libTestUtils::TestProjectContext context;

    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = libTestUtils::TestProcessorFactory::getThisIdentifier();
    data.m_factoryVersion = 1;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestRecordFeature::s_intIdInitializer);

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    babelwires::IdentifierRegistryScope identifierRegistry;

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::ProcessorElement>());
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_TRUE(featureElement->getInputFeature()->as<const libTestUtils::TestRecordFeature>());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::ProcessorElementData>());

    const libTestUtils::TestRecordFeature* inputFeature =
        static_cast<const libTestUtils::TestRecordFeature*>(featureElement->getInputFeature());

    EXPECT_EQ(inputFeature->m_intFeature->get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}
