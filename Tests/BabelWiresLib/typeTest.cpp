#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeTest, typeAndValue)
{
    // This mostly just exercises the API.

    testUtils::TestType testType;
    auto value = testType.createValue();
    EXPECT_TRUE(value);
    EXPECT_TRUE(value->as<testUtils::TestValue>());
    EXPECT_TRUE(value->isValid(testType));

    testUtils::TestEnum testEnum;
    EXPECT_FALSE(value->isValid(testEnum));
}
