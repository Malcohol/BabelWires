#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeTest, typeAndValue)
{
    babelwires::TypeSystem typeSystem;
    testUtils::TestLog log;

    // This mostly just exercises the API.

    babelwires::StringType type;
    auto [valueHolder, value] = type.createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    EXPECT_TRUE(value.as<babelwires::StringValue>());
    EXPECT_TRUE(type.isValidValue(typeSystem, value));

    testDomain::TestEnum testEnum;
    EXPECT_FALSE(testEnum.isValidValue(typeSystem, value));
}

TEST(TypeTest, typePtrTest)
{
    babelwires::TypeSystem typeSystem;
    testUtils::TestLog log;

    typeSystem.addType<babelwires::StringType>();
    typeSystem.addTypeConstructor<babelwires::ArrayTypeConstructor>();

    babelwires::TypeExp arrayOfStringsExp = babelwires::ArrayTypeConstructor::makeTypeExp(
        babelwires::TypeExp(babelwires::StringType::getThisIdentifier()), 0, 10); 

    babelwires::WeakTypePtr weakTypePtr;
    {
        babelwires::TypePtr typePtr = arrayOfStringsExp.resolve(typeSystem);
        EXPECT_TRUE(typePtr);
        babelwires::TypePtr typePtr2 = arrayOfStringsExp.resolve(typeSystem);
        // Constructing the same type twice gives the same pointer.
        EXPECT_EQ(typePtr, typePtr2);
        weakTypePtr = typePtr;
        EXPECT_FALSE(weakTypePtr.expired());
    }
    // Confirm TypeConstructor caches do not keep types alive longer than necessary.
    EXPECT_TRUE(weakTypePtr.expired());

    {
        // Confirm that the type can be recreated.
        babelwires::TypePtr typePtr = arrayOfStringsExp.resolve(typeSystem);
        EXPECT_TRUE(typePtr);
        weakTypePtr = typePtr;
        EXPECT_FALSE(weakTypePtr.expired());
    }
    EXPECT_TRUE(weakTypePtr.expired());
}
