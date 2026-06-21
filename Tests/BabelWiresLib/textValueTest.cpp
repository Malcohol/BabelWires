#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(TextValueTest, basics) {
    babelwires::TextValue textValue;
    EXPECT_EQ(textValue.get(), babelwires::Text());

    textValue.set(u8"Hello");
    EXPECT_EQ(textValue.get(), u8"Hello");

    babelwires::TextValue textValue2(u8"Goodbye");
    EXPECT_EQ(textValue2.get(), u8"Goodbye");
}

TEST(TextValueTest, serialization) {
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

TEST(TextValueTest, clone) {
    babelwires::TextValue textValue(u8"Plop");
    auto clone = textValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), u8"Plop");
}

TEST(TextValueTest, visitors) {
    babelwires::TextValue textValue(u8"Splash");
    EXPECT_FALSE(textValue.canContainFilePaths());
    EXPECT_FALSE(textValue.canContainIdentifiers());
}

TEST(TextValueTest, hash) {
    babelwires::TextValue textValue;
    std::size_t hash0 = textValue.getHash();

    textValue.set(u8"ping");
    std::size_t hash1 = textValue.getHash();

    textValue.set(babelwires::Text());
    std::size_t hash2 = textValue.getHash();

    babelwires::TextValue textValue2(u8"ping");
    std::size_t hash3 = textValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(TextValueTest, equality) {
    babelwires::TextValue textValue0;
    babelwires::TextValue textValue1(u8"pong");
    babelwires::TextValue textValue2(u8"pong");
    
    EXPECT_NE(textValue0, textValue1);
    EXPECT_EQ(textValue1, textValue2);

    textValue1.set(babelwires::Text());
    EXPECT_EQ(textValue0, textValue1);
}

TEST(TextValueTest, toString) {
    babelwires::TextValue textValue;
    
    EXPECT_EQ(textValue.toString(), "");
    
    textValue.set(u8"ping");
    EXPECT_EQ(textValue.toString(), "ping");
}
