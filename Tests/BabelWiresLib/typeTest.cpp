#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeTest, typeAndValue)
{
    babelwires::TypeSystem typeSystem;
    // This mostly just exercises the API.

    testUtils::TestType testType;
    auto [valueHolder, value] = testType.createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    EXPECT_TRUE(value.as<testUtils::TestValue>());
    EXPECT_TRUE(testType.isValidValue(value));

    testUtils::TestEnum testEnum;
    EXPECT_FALSE(testEnum.isValidValue(value));
}
