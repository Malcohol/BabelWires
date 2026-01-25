#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testProcessor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

namespace {
    void setCommonFields(babelwires::NodeData& data) {
        data.m_id = 100;
        data.m_uiData.m_uiPosition.m_x = 12;
        data.m_uiData.m_uiPosition.m_y = -44;
        data.m_uiData.m_uiSize.m_width = 300;
        data.m_expandedPaths.emplace_back(*babelwires::Path::deserializeFromString("aa/bb"));
    }

    void checkCommonFields(const babelwires::NodeData& data, bool testExpandedPaths = true) {
        EXPECT_EQ(data.m_id, 100);
        EXPECT_EQ(data.m_uiData.m_uiPosition.m_x, 12);
        EXPECT_EQ(data.m_uiData.m_uiPosition.m_y, -44);
        EXPECT_EQ(data.m_uiData.m_uiSize.m_width, 300);
        if (testExpandedPaths) {
            ASSERT_EQ(data.m_expandedPaths.size(), 1);
            EXPECT_EQ(data.m_expandedPaths[0], *babelwires::Path::deserializeFromString("aa/bb"));
        } else {
            EXPECT_EQ(data.m_expandedPaths.size(), 0);
        }
    }

    void setModifiers(babelwires::NodeData& data, const babelwires::Path& path) {
        auto newMod = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(12));
        newMod->m_targetPath = path;
        data.m_modifiers.emplace_back(std::move(newMod));
    }

    void checkModifiers(const babelwires::NodeData& data, const babelwires::Path& path) {
        EXPECT_EQ(data.m_modifiers.size(), 1);
        EXPECT_NE(data.m_modifiers[0]->tryAs<babelwires::ValueAssignmentData>(), nullptr);
        const auto& mod = static_cast<const babelwires::ValueAssignmentData&>(*data.m_modifiers[0]);
        EXPECT_EQ(mod.m_targetPath, path);
        EXPECT_EQ(mod.getValue()->tryAs<babelwires::IntValue>()->get(), 12);
    }

    void setModifiers(babelwires::NodeData& data, babelwires::ShortId fieldId) {
        setModifiers(data, babelwires::Path({fieldId}));
    }

    void checkModifiers(const babelwires::NodeData& data, babelwires::ShortId fieldId) {
        checkModifiers(data, babelwires::Path({fieldId}));
    }
} // namespace

TEST(ElementDataTest, sourceFileDataClone) {
    babelwires::SourceFileNodeData data;
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
    babelwires::SourceFileNodeData data;
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
        babelwires::SourceFileNodeData data;
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
    auto dataPtr = deserializer.deserializeObject<babelwires::SourceFileNodeData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    EXPECT_EQ(dataPtr->m_filePath, std::filesystem::path("/a/b/c/foo.bar"));
}

TEST(ElementDataTest, sourceFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    // Create a test file.
    std::ostringstream tempFileName;
    tempFileName << "foo." << testDomain::TestSourceFileFormat::getFileExtension();
    testUtils::TempFilePath tempFilePath(tempFileName.str());
    {
        auto targetFileFormat = std::make_unique<testDomain::TestTargetFileFormat>();
        auto fileFeature = std::make_unique<babelwires::ValueTreeRoot>(testEnvironment.m_projectContext.m_typeSystem,
                                                                       testDomain::getTestFileType());
        fileFeature->setToDefault();
        testDomain::TestSimpleRecordType::Instance instance{fileFeature->getChild(0)->as<babelwires::ValueTreeNode>()};
        instance.getintR0().set(14);
        targetFileFormat->writeToFile(testEnvironment.m_projectContext, testEnvironment.m_log, *fileFeature, tempFilePath);
    }

    // Create sourceFileData which expect to be able to load the file.
    babelwires::SourceFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestSourceFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = tempFilePath;

    const babelwires::Path expandedPath = *babelwires::Path::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::Node> node =
        data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    EXPECT_TRUE(node->tryAs<babelwires::SourceFileNode>());
    EXPECT_TRUE(node->getOutput());
    EXPECT_EQ(node->getNodeData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(node->getNodeData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(node->getNodeData().tryAs<babelwires::SourceFileNodeData>());
    EXPECT_EQ(static_cast<const babelwires::SourceFileNodeData&>(node->getNodeData()).m_filePath, data.m_filePath);

    testDomain::TestSimpleRecordType::ConstInstance instance(*node->getOutput()->getChild(0));
    EXPECT_EQ(instance.getintR0().get(), 14);

    EXPECT_TRUE(node->isExpanded(expandedPath));
}

TEST(ElementDataTest, targetFileDataClone) {
    babelwires::TargetFileNodeData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testDomain::getTestFileElementPathToInt0());
    data.m_filePath = "/a/b/c/foo.bar";
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, testDomain::getTestFileElementPathToInt0());
    EXPECT_EQ(clone->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCustomClone) {
    babelwires::TargetFileNodeData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testDomain::getTestFileElementPathToInt0());
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
        babelwires::TargetFileNodeData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        setModifiers(data, testDomain::getTestFileElementPathToInt0());
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
    auto dataPtr = deserializer.deserializeObject<babelwires::TargetFileNodeData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, testDomain::getTestFileElementPathToInt0());
    EXPECT_EQ(dataPtr->m_filePath, "/a/b/c/foo.bar");
}

TEST(ElementDataTest, targetFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TargetFileNodeData data;
    data.m_factoryIdentifier = testDomain::TestTargetFileFormat::getThisIdentifier();
    data.m_factoryVersion = 1;
    data.m_filePath = "foo";
    setCommonFields(data);
    setModifiers(data, testDomain::getTestFileElementPathToInt0());

    const babelwires::Path expandedPath = *babelwires::Path::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::Node> node =
        data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    EXPECT_TRUE(node->tryAs<babelwires::TargetFileNode>());
    EXPECT_TRUE(node->getInput());
    EXPECT_EQ(node->getNodeData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(node->getNodeData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(node->getNodeData().tryAs<babelwires::TargetFileNodeData>());
    EXPECT_EQ(static_cast<const babelwires::TargetFileNodeData&>(node->getNodeData()).m_filePath, data.m_filePath);

    testDomain::TestSimpleRecordType::ConstInstance instance(*node->getInput()->getChild(0));
    EXPECT_EQ(instance.getintR0().get(), 12);

    EXPECT_TRUE(node->isExpanded(expandedPath));
}

TEST(ElementDataTest, processorDataClone) {
    babelwires::ProcessorNodeData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testDomain::TestSimpleRecordType::s_int0IdInitializer);
    auto clone = data.clone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone);
    checkModifiers(*clone, testDomain::TestSimpleRecordType::s_int0IdInitializer);
}

TEST(ElementDataTest, processorDataCustomClone) {
    babelwires::ProcessorNodeData data;
    data.m_factoryIdentifier = "foo";
    data.m_factoryVersion = 14;
    setCommonFields(data);
    setModifiers(data, testDomain::TestSimpleRecordType::s_int0IdInitializer);
    auto clone = data.customClone();
    EXPECT_EQ(data.m_factoryIdentifier, "foo");
    EXPECT_EQ(data.m_factoryVersion, 14);
    checkCommonFields(*clone, false);
    EXPECT_TRUE(clone->m_modifiers.empty());
}

TEST(ElementDataTest, processorDataSerialize) {
    std::string serializedContents;
    {
        babelwires::ProcessorNodeData data;
        data.m_factoryIdentifier = "foo";
        setCommonFields(data);
        setModifiers(data, testDomain::TestSimpleRecordType::s_int0IdInitializer);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ProcessorNodeData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_factoryIdentifier, "foo");
    checkCommonFields(*dataPtr);
    checkModifiers(*dataPtr, testDomain::TestSimpleRecordType::s_int0IdInitializer);
}

TEST(ElementDataTest, processorDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ProcessorNodeData data;
    data.m_factoryIdentifier = testDomain::TestProcessor::getFactoryIdentifier();
    data.m_factoryVersion = 1;
    setCommonFields(data);
    setModifiers(data, testDomain::TestProcessorInputOutputType::s_intIdInitializer);

    const babelwires::Path expandedPath = *babelwires::Path::deserializeFromString("cc/dd");
    data.m_expandedPaths.emplace_back(expandedPath);

    std::unique_ptr<const babelwires::Node> node =
        data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    EXPECT_TRUE(node);
    ASSERT_FALSE(node->isFailed());
    EXPECT_TRUE(node->tryAs<babelwires::ProcessorNode>());
    EXPECT_TRUE(node->getInput());
    EXPECT_EQ(node->getNodeData().m_factoryIdentifier, data.m_factoryIdentifier);
    EXPECT_EQ(node->getNodeData().m_factoryVersion, data.m_factoryVersion);
    EXPECT_TRUE(node->getNodeData().tryAs<babelwires::ProcessorNodeData>());

    const auto& inputFeature = *node->getInput();

    testDomain::TestProcessorInputOutputType::ConstInstance input{inputFeature};

    EXPECT_EQ(input.getInt().get(), 12);

    EXPECT_TRUE(node->isExpanded(expandedPath));
}
