#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(StringValueTest, basics) {
    babelwires::TextValue textValue;
    EXPECT_EQ(textValue.get(), babelwires::Text());

    textValue.set(u8"Hello");
    EXPECT_EQ(textValue.get(), u8"Hello");

    babelwires::TextValue stringValue2(u8"Goodbye");
    EXPECT_EQ(stringValue2.get(), u8"Goodbye");
}

TEST(StringValueTest, serialization) {
    std::string serializedContents;
    {
        babelwires::TextValue textValue(u8"Boing");

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        auto serializer = babelwires::UserDocumentSerializationFactory::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(textValue);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestEnvironment testEnvironment;
    auto deserializer =
        babelwires::UserDocumentSerializationFactory::createDeserializer(testEnvironment.m_deserializationReg, testEnvironment.m_log);
    ASSERT_NE(deserializer, nullptr);
    ASSERT_TRUE(deserializer->parse(serializedContents));
    auto dataPtrResult = deserializer->deserializeObject<babelwires::TextValue>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer->finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get(), u8"Boing");
}

TEST(StringValueTest, clone) {
    babelwires::TextValue textValue(u8"Plop");
    auto clone = textValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), u8"Plop");
}

TEST(StringValueTest, visitors) {
    babelwires::TextValue textValue(u8"Splash");
    EXPECT_FALSE(textValue.canContainFilePaths());
    EXPECT_FALSE(textValue.canContainIdentifiers());
}

TEST(StringValueTest, hash) {
    babelwires::TextValue textValue;
    std::size_t hash0 = textValue.getHash();

    textValue.set(u8"ping");
    std::size_t hash1 = textValue.getHash();

    textValue.set(babelwires::Text());
    std::size_t hash2 = textValue.getHash();

    babelwires::TextValue stringValue2(u8"ping");
    std::size_t hash3 = stringValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(StringValueTest, equality) {
    babelwires::TextValue stringValue0;
    babelwires::TextValue stringValue1(u8"pong");
    babelwires::TextValue stringValue2(u8"pong");
    
    EXPECT_NE(stringValue0, stringValue1);
    EXPECT_EQ(stringValue1, stringValue2);

    stringValue1.set(babelwires::Text());
    EXPECT_EQ(stringValue0, stringValue1);
}

TEST(StringValueTest, toString) {
    babelwires::TextValue textValue;
    
    EXPECT_EQ(textValue.toString(), "");
    
    textValue.set(u8"ping");
    EXPECT_EQ(textValue.toString(), "ping");
}
