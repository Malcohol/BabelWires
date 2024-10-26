#include <gtest/gtest.h>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(StringValueTest, basics) {
    babelwires::StringValue stringValue;
    EXPECT_EQ(stringValue.get(), std::string());

    stringValue.set("Hello");
    EXPECT_EQ(stringValue.get(), "Hello");

    babelwires::StringValue stringValue2("Goodbye");
    EXPECT_EQ(stringValue2.get(), "Goodbye");
}

TEST(StringValueTest, serialization) {
    std::string serializedContents;
    {
        babelwires::StringValue stringValue("Boing");

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        babelwires::XmlSerializer serializer;
        serializer.serializeObject(stringValue);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::StringValue>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get(), "Boing");
}

TEST(StringValueTest, clone) {
    babelwires::StringValue stringValue("Plop");
    auto clone = stringValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), "Plop");  
}

TEST(StringValueTest, visitors) {
    babelwires::StringValue stringValue("Splash");
    EXPECT_FALSE(stringValue.canContainFilePaths());
    EXPECT_FALSE(stringValue.canContainIdentifiers());
}

TEST(StringValueTest, hash) {
    babelwires::StringValue stringValue;
    std::size_t hash0 = stringValue.getHash();

    stringValue.set("ping");
    std::size_t hash1 = stringValue.getHash();

    stringValue.set(std::string());
    std::size_t hash2 = stringValue.getHash();

    babelwires::StringValue stringValue2("ping");
    std::size_t hash3 = stringValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(StringValueTest, equality) {
    babelwires::StringValue stringValue0;
    babelwires::StringValue stringValue1("pong");
    babelwires::StringValue stringValue2("pong");
    
    EXPECT_NE(stringValue0, stringValue1);
    EXPECT_EQ(stringValue1, stringValue2);

    stringValue1.set(std::string());
    EXPECT_EQ(stringValue0, stringValue1);
}

TEST(StringValueTest, toString) {
    babelwires::StringValue stringValue;
    
    EXPECT_EQ(stringValue.toString(), "");
    
    stringValue.set("ping");
    EXPECT_EQ(stringValue.toString(), "ping");
}
