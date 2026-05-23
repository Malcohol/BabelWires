#include <Tests/BaseLib/TestData/serializableClasses.hpp>
#include <Tests/TestUtils/XmlSerialization/xmlSerializer.hpp>

#include <gtest/gtest.h>

using namespace babelwires;

using testData::A;
using testData::KeyedContainer;

TEST(XmlSerializationTest, xmlSerializerRejectsReservedMetadataPrefixForObjectKeys) {
    EXPECT_DEATH(
        {
            A a;
            XmlSerializer serializer;
            serializer.serializeObject(a, "meta:illegal");
        },
        "valid identifier");
}

TEST(XmlSerializationTest, xmlSerializerRejectsReservedMetadataPrefixForValueKeys) {
    EXPECT_DEATH(
        {
            XmlSerializer serializer;
            serializer.serializeValue("meta:illegal", 17);
        },
        "valid identifier");
}

TEST(XmlSerializationTest, xmlSerializerUsesMetaTypeForExplicitRuntimeTypeMetadata) {
    KeyedContainer container;
    container.m_defaultKeyObject.m_x = 17;
    container.m_explicitKeyObject.m_x = 23;

    XmlSerializer serializer;
    serializer.serializeObject(container);
    std::ostringstream os;
    serializer.write(os);

    const std::string serializedContents = os.str();
    EXPECT_NE(serializedContents.find("<contents xmlns:meta=\"urn:babelwires:serialization-meta\""),
              std::string::npos);
    EXPECT_NE(serializedContents.find("<renamedA meta:type=\"A\""), std::string::npos);
    EXPECT_EQ(serializedContents.find("typeName=\"A\""), std::string::npos);
}