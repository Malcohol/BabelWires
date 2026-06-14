#include <gtest/gtest.h>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(StringValueTest, basics) {
    babelwires::StringValue stringValue;
    EXPECT_EQ(stringValue.get(), babelwires::Text());

    stringValue.set(u8"Hello");
    EXPECT_EQ(stringValue.get(), u8"Hello");

    babelwires::StringValue stringValue2(u8"Goodbye");
    EXPECT_EQ(stringValue2.get(), u8"Goodbye");
}

TEST(StringValueTest, serialization) {
    std::string serializedContents;
    {
        babelwires::StringValue stringValue(u8"Boing");

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        auto serializer = babelwires::UserDocumentSerializationFactory::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(stringValue);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestEnvironment testEnvironment;
    auto deserializer =
        babelwires::UserDocumentSerializationFactory::createDeserializer(testEnvironment.m_deserializationReg, testEnvironment.m_log);
    ASSERT_NE(deserializer, nullptr);
    ASSERT_TRUE(deserializer->parse(serializedContents));
    auto dataPtrResult = deserializer->deserializeObject<babelwires::StringValue>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer->finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get(), u8"Boing");
}

TEST(StringValueTest, clone) {
    babelwires::StringValue stringValue(u8"Plop");
    auto clone = stringValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), u8"Plop");
}

TEST(StringValueTest, visitors) {
    babelwires::StringValue stringValue(u8"Splash");
    EXPECT_FALSE(stringValue.canContainFilePaths());
    EXPECT_FALSE(stringValue.canContainIdentifiers());
}

TEST(StringValueTest, hash) {
    babelwires::StringValue stringValue;
    std::size_t hash0 = stringValue.getHash();

    stringValue.set(u8"ping");
    std::size_t hash1 = stringValue.getHash();

    stringValue.set(babelwires::Text());
    std::size_t hash2 = stringValue.getHash();

    babelwires::StringValue stringValue2(u8"ping");
    std::size_t hash3 = stringValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(StringValueTest, equality) {
    babelwires::StringValue stringValue0;
    babelwires::StringValue stringValue1(u8"pong");
    babelwires::StringValue stringValue2(u8"pong");
    
    EXPECT_NE(stringValue0, stringValue1);
    EXPECT_EQ(stringValue1, stringValue2);

    stringValue1.set(babelwires::Text());
    EXPECT_EQ(stringValue0, stringValue1);
}

TEST(StringValueTest, toString) {
    babelwires::StringValue stringValue;
    
    EXPECT_EQ(stringValue.toString(), "");
    
    stringValue.set(u8"ping");
    EXPECT_EQ(stringValue.toString(), "ping");
}
