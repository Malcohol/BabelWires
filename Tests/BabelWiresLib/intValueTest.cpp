#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(IntValueTest, basics) {
    babelwires::IntValue intValue;
    EXPECT_EQ(intValue.get(), 0);

    intValue.set(10);
    EXPECT_EQ(intValue.get(), 10);

    babelwires::IntValue intValue2(-12);
    EXPECT_EQ(intValue2.get(), -12);
}

TEST(IntValueTest, serialization) {
    std::string serializedContents;
    {
        babelwires::IntValue intValue(104);

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        babelwires::XmlSerializer serializer;
        serializer.serializeObject(intValue);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::IntValue>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get(), 104);
}

TEST(IntValueTest, clone) {
    babelwires::IntValue intValue(-7777);
    auto clone = intValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), -7777);  
}

TEST(IntValueTest, visitors) {
    babelwires::IntValue intValue(13);
    EXPECT_FALSE(intValue.canContainFilePaths());
    EXPECT_FALSE(intValue.canContainIdentifiers());
}

TEST(IntValueTest, hash) {
    babelwires::IntValue intValue;
    std::size_t hash0 = intValue.getHash();

    intValue.set(127);
    std::size_t hash1 = intValue.getHash();

    intValue.set(0);
    std::size_t hash2 = intValue.getHash();

    babelwires::IntValue intValue2(127);
    std::size_t hash3 = intValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(IntValueTest, equality) {
    babelwires::IntValue intValue0;
    babelwires::IntValue intValue1(12);
    babelwires::IntValue intValue2(12);
    
    EXPECT_NE(intValue0, intValue1);
    EXPECT_EQ(intValue1, intValue2);

    intValue1.set(0);
    EXPECT_EQ(intValue0, intValue1);
}

TEST(IntValueTest, toString) {
    babelwires::IntValue intValue;
    
    EXPECT_EQ(intValue.toString(), "0");
    
    intValue.set(-17);
    EXPECT_EQ(intValue.toString(), "-17");
}
