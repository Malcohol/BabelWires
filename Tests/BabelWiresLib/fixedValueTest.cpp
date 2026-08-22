#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

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
