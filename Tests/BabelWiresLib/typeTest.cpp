#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/TestUtils/equalSets.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeTest, typeAndValue)
{
    babelwires::TypeSystem typeSystem;
    // This mostly just exercises the API.

    babelwires::StringType type;
    auto [valueHolder, value] = type.createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    EXPECT_TRUE(value.as<babelwires::StringValue>());
    EXPECT_TRUE(type.isValidValue(typeSystem, value));

    testDomain::TestEnum testEnum;
    EXPECT_FALSE(testEnum.isValidValue(typeSystem, value));
}
