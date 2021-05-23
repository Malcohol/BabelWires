#include <gtest/gtest.h>

#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/FeatureElements/processorElement.hpp"
#include "BabelWires/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWires/Project/FeatureElements/targetFileElement.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Features/numericFeature.hpp"

#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProcessor.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

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
        EXPECT_NE(dynamic_cast<const babelwires::IntValueAssignmentData*>(data.m_modifiers[0].get()), nullptr);
        const auto& mod = static_cast<const babelwires::IntValueAssignmentData&>(*data.m_modifiers[0]);
        EXPECT_EQ(mod.m_pathToFeature, babelwires::FeaturePath::deserializeFromString(pathStep));
        EXPECT_EQ(mod.m_value, 12);
    }
} // namespace

TEST(ElementDataTest, sourceFileDataClone) {
    babelwires::SourceFileData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    data.m_absoluteFilePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    EXPECT_EQ(clone->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, sourceFileDataCustomClone) {
    babelwires::SourceFileData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    data.m_absoluteFilePath = "/a/b/c/foo.bar";
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_EQ(clone->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, sourceFileDataSerialize) {
    std::string serializedContents;
    {
        babelwires::SourceFileData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        data.m_absoluteFilePath = "/a/b/c/foo.bar";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::SourceFileData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    EXPECT_EQ(dataPtr->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, sourceFileDataCreateElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
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
    babelwires::SourceFileData data;
    data.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_absoluteFilePath = tempFilePath;

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(dynamic_cast<const babelwires::SourceFileElement*>(featureElement.get()));
    EXPECT_TRUE(featureElement->getOutputFeature());
    EXPECT_TRUE(dynamic_cast<const libTestUtils::TestFileFeature*>(featureElement->getOutputFeature()));
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(dynamic_cast<const babelwires::SourceFileData*>(&featureElement->getElementData()));
    EXPECT_EQ(static_cast<const babelwires::SourceFileData&>(featureElement->getElementData()).m_absoluteFilePath,
              data.m_absoluteFilePath);

    const libTestUtils::TestFileFeature* inputFeature =
        static_cast<const libTestUtils::TestFileFeature*>(featureElement->getOutputFeature());

    EXPECT_EQ(inputFeature->getFileFormatIdentifier(), libTestUtils::TestSourceFileFormat::getThisIdentifier());
    EXPECT_EQ(inputFeature->m_intChildFeature->get(), 14);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, targetFileDataClone) {
    babelwires::TargetFileData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
    data.m_absoluteFilePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, libTestUtils::TestFileFeature::s_intChildInitializer);
    EXPECT_EQ(clone->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCustomClone) {
    babelwires::TargetFileData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
    data.m_absoluteFilePath = "/a/b/c/foo.bar";
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_TRUE(clone->m_modifiers.empty());
    EXPECT_EQ(clone->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataSerialize) {
    std::string serializedContents;
    {
        babelwires::TargetFileData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);
        data.m_absoluteFilePath = "/a/b/c/foo.bar";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::TargetFileData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, libTestUtils::TestFileFeature::s_intChildInitializer);
    EXPECT_EQ(dataPtr->m_absoluteFilePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCreateElement) {
    libTestUtils::TestProjectContext context;

    babelwires::TargetFileData data;
    data.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_absoluteFilePath = "foo";
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestFileFeature::s_intChildInitializer);

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(dynamic_cast<const babelwires::TargetFileElement*>(featureElement.get()));
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_TRUE(dynamic_cast<const libTestUtils::TestFileFeature*>(featureElement->getInputFeature()));
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(dynamic_cast<const babelwires::TargetFileData*>(&featureElement->getElementData()));
    EXPECT_EQ(static_cast<const babelwires::TargetFileData&>(featureElement->getElementData()).m_absoluteFilePath,
              data.m_absoluteFilePath);

    const libTestUtils::TestFileFeature* inputFeature =
        static_cast<const libTestUtils::TestFileFeature*>(featureElement->getInputFeature());

    EXPECT_EQ(inputFeature->getFileFormatIdentifier(), libTestUtils::TestSourceFileFormat::getThisIdentifier());
    EXPECT_EQ(inputFeature->m_intChildFeature->get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, processorDataClone) {
    babelwires::ProcessorData data;
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
    babelwires::ProcessorData data;
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
        babelwires::ProcessorData data;
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
    auto dataPtr = deserializer.deserializeObject<babelwires::ProcessorData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, libTestUtils::TestRecordFeature::s_intIdInitializer);
}

TEST(ElementDataTest, processorDataCreateElement) {
    libTestUtils::TestProjectContext context;

    babelwires::ProcessorData data;
    data.m_factoryIdentifier = libTestUtils::TestProcessorFactory::getThisIdentifier();
    data.m_factoryVersion = 1;
    setCommonFields(data);
    setModifiers(data, libTestUtils::TestRecordFeature::s_intIdInitializer);

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(context.m_projectContext, context.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(dynamic_cast<const babelwires::ProcessorElement*>(featureElement.get()));
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_TRUE(dynamic_cast<const libTestUtils::TestRecordFeature*>(featureElement->getInputFeature()));
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(dynamic_cast<const babelwires::ProcessorData*>(&featureElement->getElementData()));

    const libTestUtils::TestRecordFeature* inputFeature =
        static_cast<const libTestUtils::TestRecordFeature*>(featureElement->getInputFeature());

    EXPECT_EQ(inputFeature->m_intFeature->get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}
