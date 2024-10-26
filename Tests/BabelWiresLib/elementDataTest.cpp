#include <gtest/gtest.h>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ProcessorElement/processorElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ProcessorElement/processorElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

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

    void setModifiers(babelwires::ElementData& data, const babelwires::FeaturePath& path) {
        auto newMod = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(12));
        newMod->m_pathToFeature = path;
        data.m_modifiers.emplace_back(std::move(newMod));
    }

    void checkModifiers(const babelwires::ElementData& data, const babelwires::FeaturePath& path) {
        EXPECT_EQ(data.m_modifiers.size(), 1);
        EXPECT_NE(data.m_modifiers[0]->as<babelwires::ValueAssignmentData>(), nullptr);
        const auto& mod = static_cast<const babelwires::ValueAssignmentData&>(*data.m_modifiers[0]);
        EXPECT_EQ(mod.m_pathToFeature, path);
        EXPECT_EQ(mod.getValue()->as<babelwires::IntValue>()->get(), 12);
    }

    void setModifiers(babelwires::ElementData& data, babelwires::ShortId fieldId) {
        setModifiers(data, babelwires::FeaturePath({babelwires::PathStep(fieldId)}));
    }

    void checkModifiers(const babelwires::ElementData& data, babelwires::ShortId fieldId) {
        checkModifiers(data, babelwires::FeaturePath({babelwires::PathStep(fieldId)}));
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
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName;
    tempFileName << "foo." << testUtils::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());
    {
        std::ofstream tempFile = tempFilePath.openForWriting();

        auto targetFileFormat = std::make_unique<testUtils::TestTargetFileFormat>();
        auto fileFeature = std::make_unique<babelwires::SimpleValueFeature>(testEnvironment.m_projectContext.m_typeSystem, testUtils::getTestFileType());
        fileFeature->setToDefault();
        testUtils::TestSimpleRecordType::Instance instance{fileFeature->getFeature(0)->is<babelwires::Feature>()};
        instance.getintR0().set(14);
        targetFileFormat->writeToFile(testEnvironment.m_projectContext, testEnvironment.m_log, *fileFeature, tempFile);
    }

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileElementData data;
    data.m_factoryIdentifier = testUtils::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::SourceFileElement>());
    EXPECT_TRUE(featureElement->getOutputFeature());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::SourceFileElementData>());
    EXPECT_EQ(static_cast<const babelwires::SourceFileElementData&>(featureElement->getElementData()).m_filePath,
              data.m_filePath);

    testUtils::TestSimpleRecordType::ConstInstance instance(featureElement->getOutputFeature()->is<babelwires::Feature>().getFeature(0)->is<babelwires::Feature>());
    EXPECT_EQ(instance.getintR0().get(), 14);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, targetFileDataClone) {
    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testUtils::getTestFileElementPathToInt0());
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, testUtils::getTestFileElementPathToInt0());
    EXPECT_EQ(clone->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCustomClone) {
    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testUtils::getTestFileElementPathToInt0());
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
        setModifiers(data, testUtils::getTestFileElementPathToInt0());
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
    checkModifiers(*dataPtr, testUtils::getTestFileElementPathToInt0());
    EXPECT_EQ(dataPtr->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TargetFileElementData data;
    data.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = "foo";
    setCommonFields(data);
    setModifiers(data, testUtils::getTestFileElementPathToInt0());

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::TargetFileElement>());
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::TargetFileElementData>());
    EXPECT_EQ(static_cast<const babelwires::TargetFileElementData&>(featureElement->getElementData()).m_filePath,
              data.m_filePath);

    testUtils::TestSimpleRecordType::ConstInstance instance(featureElement->getInputFeature()->is<babelwires::Feature>().getFeature(0)->is<babelwires::Feature>());
    EXPECT_EQ(instance.getintR0().get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}

TEST(ElementDataTest, processorDataClone) {
    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testUtils::TestSimpleRecordType::s_int0IdInitializer);
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, testUtils::TestSimpleRecordType::s_int0IdInitializer);
}

TEST(ElementDataTest, processorDataCustomClone) {
    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testUtils::TestSimpleRecordType::s_int0IdInitializer);
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
        setModifiers(data, testUtils::TestSimpleRecordType::s_int0IdInitializer);

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
    checkModifiers(*dataPtr, testUtils::TestSimpleRecordType::s_int0IdInitializer);
}

TEST(ElementDataTest, processorDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = testUtils::TestProcessor::getFactoryIdentifier();
    data.m_factoryVersion = 1;
    setCommonFields(data);
    setModifiers(data, testUtils::TestProcessorInputOutputType::s_intIdInitializer);

    const babelwires::FeaturePath expandedPath = babelwires::FeaturePath::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::FeatureElement> featureElement =
        data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->as<babelwires::ProcessorElement>());
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_EQ(featureElement->getElementData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(featureElement->getElementData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(featureElement->getElementData().as<babelwires::ProcessorElementData>());

    const auto& inputFeature = featureElement->getInputFeature()->is<babelwires::Feature>();

    testUtils::TestProcessorInputOutputType::ConstInstance input{inputFeature};

    EXPECT_EQ(input.getInt().get(), 12);

    EXPECT_TRUE(featureElement->isExpanded(expandedPath));
}
