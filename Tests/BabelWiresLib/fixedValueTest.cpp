#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>
#include <BabelWiresLib/Types/Fixed/fixedType.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(FixedValueTest, basics) {
    babelwires::Fixed fixedValue;
    EXPECT_EQ(fixedValue.getNumerator(), 0);
    EXPECT_EQ(fixedValue.getPrecision(), 0);

    fixedValue = babelwires::Fixed(123, 2);
    EXPECT_EQ(fixedValue.getNumerator(), 123);
    EXPECT_EQ(fixedValue.getPrecision(), 2);

    fixedValue = babelwires::Fixed(-0, 3);
    EXPECT_EQ(fixedValue.getNumerator(), 0);
    EXPECT_EQ(fixedValue.getPrecision(), 3);
}

TEST(FixedValueTest, serializationRoundTrip) {
    std::string serializedContents;
    {
        babelwires::FixedValue value(babelwires::Fixed(125, 2));
        auto serializer = babelwires::UserDocumentSerializationFactory::createSerializer();
        ASSERT_NE(serializer, nullptr);
        serializer->serializeObject(value);
        std::ostringstream os;
        serializer->write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestEnvironment testEnvironment;
    auto deserializer =
        babelwires::UserDocumentSerializationFactory::createDeserializer(testEnvironment.m_deserializationReg, testEnvironment.m_log);
    ASSERT_NE(deserializer, nullptr);
    ASSERT_TRUE(deserializer->parse(serializedContents));
    auto dataPtrResult = deserializer->deserializeObject<babelwires::FixedValue>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer->finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->get().getNumerator(), 125);
    EXPECT_EQ(dataPtr->get().getPrecision(), 2);
}

TEST(FixedValueTest, equalityAndHash) {
    babelwires::FixedValue valueA(babelwires::Fixed(12, 3));
    babelwires::FixedValue valueB(babelwires::Fixed(12, 3));
    babelwires::FixedValue valueC(babelwires::Fixed(12, 2));

    EXPECT_EQ(valueA, valueB);
    EXPECT_NE(valueA, valueC);
    EXPECT_EQ(valueA.getHash(), valueB.getHash());
    EXPECT_NE(valueA.getHash(), valueC.getHash());
}

TEST(FixedValueTest, stringConversion) {
    babelwires::FixedValue value(babelwires::Fixed(-1234, 3));
    EXPECT_EQ(value.toString(), "-1.234");

    babelwires::FixedValue zero(babelwires::Fixed(-0, 4));
    EXPECT_EQ(zero.toString(), "0.0000");
}
