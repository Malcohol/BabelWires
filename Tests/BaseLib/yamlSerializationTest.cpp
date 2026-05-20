#include <BaseLib/Serialization/YAML/yamlDeserializer.hpp>
#include <BaseLib/Serialization/YAML/yamlSerializer.hpp>
#include <BaseLib/Serialization/deserializationRegistry.hpp>

#include <Tests/BaseLib/TestData/serializableClasses.hpp>

#include <Tests/TestUtils/testLog.hpp>

#include <gtest/gtest.h>

#include <yaml-cpp/yaml.h>

using namespace babelwires;
using namespace testUtils;

using testData::A;
using testData::CustomValueArrayContainer;
using testData::KeyedContainer;

namespace {
    struct ScalarRecordContainer : Serializable {
        SERIALIZABLE(ScalarRecordContainer, "ScalarRecordContainer", void, 1);

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeValue("x", m_x);
            serializer.serializeValue("name", m_name);
        }

        Result deserializeContents(Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("x", m_x));
            DO_OR_ERROR(deserializer.deserializeValue("name", m_name));
            return {};
        }

        int m_x = 0;
        std::string m_name;
    };
}

TEST(YamlSerializationTest, yamlSerializerRejectsReservedMetadataPrefixForObjectKeys) {
    EXPECT_DEATH(
        {
            A a;
            YamlSerializer serializer;
            serializer.serializeObject(a, "$illegal");
        },
        "reserved for backend metadata");
}

TEST(YamlSerializationTest, yamlSerializerRejectsReservedMetadataPrefixForValueKeys) {
    EXPECT_DEATH(
        {
            YamlSerializer serializer;
            serializer.serializeValue("$illegal", 17);
        },
        "reserved for backend metadata");
}

TEST(YamlSerializationTest, yamlSerializerUsesDollarTypeForExplicitRuntimeTypeMetadata) {
    KeyedContainer container;
    container.m_defaultKeyObject.m_x = 17;
    container.m_explicitKeyObject.m_x = 23;

    YamlSerializer serializer;
    serializer.serializeObject(container);
    std::ostringstream os;
    serializer.write(os);

    const YAML::Node root = YAML::Load(os.str());
    ASSERT_TRUE(root.IsMap());
    ASSERT_TRUE(root["KeyedContainer"]);
    ASSERT_TRUE(root["KeyedContainer"]["renamedA"]);
    EXPECT_EQ(root["KeyedContainer"]["renamedA"]["$type"].as<std::string>(), "A");
    EXPECT_FALSE(root["KeyedContainer"]["A"]["$type"]);
}

TEST(YamlSerializationTest, yamlSerializerUsesPlainScalarSequencesForValueArrays) {
    CustomValueArrayContainer container;
    container.m_values = {"one", "two", "three"};

    YamlSerializer serializer;
    serializer.serializeObject(container);
    std::ostringstream os;
    serializer.write(os);

    const YAML::Node root = YAML::Load(os.str());
    ASSERT_TRUE(root.IsMap());
    const YAML::Node values = root["CustomValueArrayContainer"]["values"];
    ASSERT_TRUE(values);
    ASSERT_TRUE(values.IsSequence());
    ASSERT_EQ(values.size(), 3u);
    EXPECT_EQ(values.Style(), YAML::EmitterStyle::Flow);
    EXPECT_TRUE(values[0].IsScalar());
    EXPECT_EQ(values[0].as<std::string>(), "one");
    EXPECT_TRUE(values[1].IsScalar());
    EXPECT_EQ(values[1].as<std::string>(), "two");
    EXPECT_TRUE(values[2].IsScalar());
    EXPECT_EQ(values[2].as<std::string>(), "three");
}

TEST(YamlSerializationTest, yamlSerializerUsesFlowStyleForScalarOnlyRecords) {
    ScalarRecordContainer container;
    container.m_x = 17;
    container.m_name = "example";

    YamlSerializer serializer;
    serializer.serializeObject(container);
    std::ostringstream os;
    serializer.write(os);

    const YAML::Node root = YAML::Load(os.str());
    ASSERT_TRUE(root.IsMap());
    const YAML::Node record = root["ScalarRecordContainer"];
    ASSERT_TRUE(record);
    ASSERT_TRUE(record.IsMap());
    EXPECT_EQ(record.Style(), YAML::EmitterStyle::Flow);
    EXPECT_EQ(record["x"].as<int>(), 17);
    EXPECT_EQ(record["name"].as<std::string>(), "example");
}

TEST(YamlSerializationTest, yamlDeserializerRejectsNonScalarMapKeys) {
    const std::string serializedContents =
        "A:\n"
        "  x: 12\n"
        "  ? [bad, key]\n"
        "  : nope\n"
        "serializationMetadata:\n"
        "  - { $type: serializable, type: A, version: 1 }\n";

    TestLog log;
    DeserializationRegistry deserializationReg;
    deserializationReg.registerClass<A>();
    YamlDeserializer deserializer(deserializationReg, log);
    ASSERT_TRUE(deserializer.parse(serializedContents));
    auto objectResult = deserializer.deserializeObject<A>();
    ASSERT_FALSE(objectResult);
    EXPECT_NE(std::string_view(objectResult.error().toString()).find("non-scalar key"), std::string_view::npos);
    deserializer.finalizeOnError();
}