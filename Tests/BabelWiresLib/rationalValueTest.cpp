#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(RationalValueTest, basics) {
    babelwires::RationalValue rationalValue;
    EXPECT_EQ(rationalValue.get(), 0);

    rationalValue.set(babelwires::Rational(10, 11));
    EXPECT_EQ(rationalValue.get(), babelwires::Rational(10, 11));

    babelwires::RationalValue rationalValue2(babelwires::Rational(-14, 121));
    EXPECT_EQ(rationalValue2.get(), babelwires::Rational(-14, 121));
}

TEST(RationalValueTest, serialization) {
    std::string serializedContents;
    {
        babelwires::RationalValue rationalValue(babelwires::Rational(104, 143));

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        babelwires::XmlSerializer serializer;
        serializer.serializeObject(rationalValue);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::RationalValue>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get(), babelwires::Rational(104, 143));
}

TEST(RationalValueTest, clone) {
    babelwires::RationalValue rationalValue(babelwires::Rational(-7777, 8981));
    auto clone = rationalValue.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->get(), babelwires::Rational(-7777, 8981));  
}

TEST(RationalValueTest, visitors) {
    babelwires::RationalValue rationalValue(13);
    EXPECT_FALSE(rationalValue.canContainFilePaths());
    EXPECT_FALSE(rationalValue.canContainIdentifiers());
}

TEST(RationalValueTest, hash) {
    babelwires::RationalValue rationalValue;
    std::size_t hash0 = rationalValue.getHash();

    rationalValue.set(babelwires::Rational(1, 27));
    std::size_t hash1 = rationalValue.getHash();

    rationalValue.set(0);
    std::size_t hash2 = rationalValue.getHash();

    babelwires::RationalValue rationalValue2(babelwires::Rational(1, 27));
    std::size_t hash3 = rationalValue2.getHash();

    EXPECT_EQ(hash0, hash2);
    EXPECT_EQ(hash1, hash3);

    // Statistical.
    EXPECT_NE(hash0, hash1);
}

TEST(RationalValueTest, equality) {
    babelwires::RationalValue rationalValue0;
    babelwires::RationalValue rationalValue1(babelwires::Rational(1, 27));
    babelwires::RationalValue rationalValue2(babelwires::Rational(1, 27));
    
    EXPECT_NE(rationalValue0, rationalValue1);
    EXPECT_EQ(rationalValue1, rationalValue2);

    rationalValue1.set(0);
    EXPECT_EQ(rationalValue0, rationalValue1);
}

TEST(RationalValueTest, toString) {
    babelwires::RationalValue rationalValue;
    
    EXPECT_EQ(rationalValue.toString(), "0");
    
    rationalValue.set(babelwires::Rational(-17, 19));
    EXPECT_EQ(rationalValue.toString(), "-17/19");
}
