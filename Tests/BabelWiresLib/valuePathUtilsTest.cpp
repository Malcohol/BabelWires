#include <gtest/gtest.h>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ValuePathUtilsTest, tryFollowPathEmpty) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    const babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    babelwires::Path emptyPath;
    auto result = babelwires::tryFollowPath(typeSystem, *recordType, emptyPath, value);
    ASSERT_TRUE(result.has_value());
    const auto& [type, vh] = *result;
    EXPECT_EQ(&type, recordType.get());
    EXPECT_EQ(vh, value);
}

TEST(ValuePathUtilsTest, tryFollowLongerPath) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    const babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Follow path to array/1
    babelwires::Path pathToElem1;
    pathToElem1.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getArrayId()));
    pathToElem1.pushStep(1);
    auto result1 = babelwires::tryFollowPath(typeSystem, *recordType, pathToElem1, value);
    ASSERT_TRUE(result1.has_value());
    const auto& [type1, vh1] = *result1;
    EXPECT_NE(type1.tryAs<babelwires::IntType>(), nullptr);
}

TEST(ValuePathUtilsTest, tryFollowPathFailNonExistentField) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    const babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Try to follow a path to a non-existent field
    babelwires::Path pathToNonField;
    pathToNonField.pushStep(babelwires::PathStep("Forb"));
    auto result = babelwires::tryFollowPath(typeSystem, *recordType, pathToNonField, value);
    EXPECT_FALSE(result.has_value());
}

TEST(ValuePathUtilsTest, tryFollowPathFailOffEndOfArray) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    const babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Try to follow a path that goes past the end of an array
    babelwires::Path pathOffEndOfArray;
    pathOffEndOfArray.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getArrayId()));
    pathOffEndOfArray.pushStep(5);
    auto result = babelwires::tryFollowPath(typeSystem, *recordType, pathOffEndOfArray, value);
    EXPECT_FALSE(result.has_value());
}

TEST(ValuePathUtilsTest, tryFollowPathFailStepIntoValue) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    const babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Try to follow a path that steps into a non-compound value (an int)
    babelwires::Path pathValueAsRecord;
    pathValueAsRecord.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getInt0Id()));
    pathValueAsRecord.pushStep(babelwires::PathStep("flerm"));
    auto result1 = babelwires::tryFollowPath(typeSystem, *recordType, pathValueAsRecord, value);
    EXPECT_FALSE(result1.has_value());
}

TEST(ValuePathUtilsTest, assertFollowPathNonConstEmpty) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    babelwires::Path emptyPath;
    auto [type, vh] = babelwires::assertFollowPathNonConst(typeSystem, *recordType, emptyPath, value);
    EXPECT_EQ(&type, recordType.get());
}

TEST(ValuePathUtilsTest, assertFollowPathNonConstLongerPath) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Follow path to array/1 and modify it
    babelwires::Path pathToElem1;
    pathToElem1.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getArrayId()));
    pathToElem1.pushStep(1);
    auto [type, vh] = babelwires::assertFollowPathNonConst(typeSystem, *recordType, pathToElem1, value);
    EXPECT_NE(type.tryAs<babelwires::IntType>(), nullptr);
    vh = babelwires::IntValue(555);

    // Verify modification
    auto verifyResult = babelwires::tryFollowPath(typeSystem, *recordType, pathToElem1, value);
    ASSERT_TRUE(verifyResult.has_value());
    EXPECT_EQ(std::get<1>(*verifyResult)->as<babelwires::IntValue>().get(), 555);
}

TEST(ValuePathUtilsTest, assertVisitPathsNonConstSinglePath) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Visit a single path and increment the int value
    babelwires::Path pathToInt;
    pathToInt.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getInt0Id()));
    std::vector<babelwires::Path> paths = {pathToInt};

    babelwires::assertVisitPathsNonConst(typeSystem, *recordType, value, paths,
                                         [](const babelwires::Type& type, babelwires::ValueHolder& vh) {
                                             vh = babelwires::IntValue(17);
                                         });

    // Verify: intR0 should be 6 (was 5, incremented by 1)
    auto verifyResult = babelwires::tryFollowPath(typeSystem, *recordType, pathToInt, value);
    ASSERT_TRUE(verifyResult.has_value());
    EXPECT_EQ(std::get<1>(*verifyResult)->as<babelwires::IntValue>().get(), 17);
}

TEST(ValuePathUtilsTest, assertVisitPathsNonConstMultiplePaths) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Visit multiple paths and increment all int values at those paths
    babelwires::Path pathToInt0;
    pathToInt0.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getInt0Id()));

    babelwires::Path pathToInt1;
    pathToInt1.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getInt1Id()));

    babelwires::Path pathToSubRecInt0;
    pathToSubRecInt0.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getSubrecordId()));
    pathToSubRecInt0.pushStep(babelwires::PathStep(testDomain::TestSimpleRecordType::getInt0Id()));

    babelwires::Path pathToArrayElem1;
    pathToArrayElem1.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getArrayId()));
    pathToArrayElem1.pushStep(1);

    std::vector<babelwires::Path> paths = {pathToInt0, pathToInt1, pathToSubRecInt0, pathToArrayElem1};

    int visitCount = 0;
    babelwires::assertVisitPathsNonConst(
        typeSystem, *recordType, value, paths,
        [&visitCount](const babelwires::Type& type, babelwires::ValueHolder& vh) {
            vh = babelwires::IntValue(1005 + visitCount);
            ++visitCount;
        });

    EXPECT_EQ(visitCount, 4);

    // Verify all visited values were incremented by 1000
    auto r0 = babelwires::tryFollowPath(typeSystem, *recordType, pathToInt0, value);
    ASSERT_TRUE(r0.has_value());
    EXPECT_EQ(std::get<1>(*r0)->as<babelwires::IntValue>().get(), 1005);

    auto r1 = babelwires::tryFollowPath(typeSystem, *recordType, pathToInt1, value);
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(std::get<1>(*r1)->as<babelwires::IntValue>().get(), 1006);

    auto rSub = babelwires::tryFollowPath(typeSystem, *recordType, pathToSubRecInt0, value);
    ASSERT_TRUE(rSub.has_value());
    EXPECT_EQ(std::get<1>(*rSub)->as<babelwires::IntValue>().get(), 1007);

    auto rArr = babelwires::tryFollowPath(typeSystem, *recordType, pathToArrayElem1, value);
    ASSERT_TRUE(rArr.has_value());
    EXPECT_EQ(std::get<1>(*rArr)->as<babelwires::IntValue>().get(), 1008);

    // Verify unvisited values are unchanged
    babelwires::Path pathToArrayElem0;
    pathToArrayElem0.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::getArrayId()));
    pathToArrayElem0.pushStep(0);
    auto rArr0 = babelwires::tryFollowPath(typeSystem, *recordType, pathToArrayElem0, value);
    ASSERT_TRUE(rArr0.has_value());
    EXPECT_TRUE(std::get<1>(*rArr0)->tryAs<babelwires::IntValue>());
}

TEST(ValuePathUtilsTest, tryFollowPathWithUnregisteredIdentifiers) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;
    const auto& recordType = typeSystem.getRegisteredType<testDomain::TestComplexRecordType>();
    babelwires::ValueHolder value = recordType->createValue(typeSystem); 

    // Use unregistered identifiers (discriminator 0) - the path should still work
    babelwires::Path pathToInt;
    pathToInt.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::s_intIdInitializer));
    EXPECT_EQ(pathToInt.getStep(0).getField().getDiscriminator(), 0);

    auto result = babelwires::tryFollowPath(typeSystem, *recordType, pathToInt, value);
    ASSERT_TRUE(result.has_value());
    auto& [type, vh] = *result;
    EXPECT_TRUE(vh->tryAs<babelwires::IntValue>());

    // Use unregistered identifiers for a nested path
    babelwires::Path pathToSubRecordInt;
    pathToSubRecordInt.pushStep(babelwires::PathStep(testDomain::TestComplexRecordType::s_subRecordIdInitializer));
    pathToSubRecordInt.pushStep(babelwires::PathStep(testDomain::TestSimpleRecordType::s_int0IdInitializer));
    EXPECT_EQ(pathToSubRecordInt.getStep(0).getField().getDiscriminator(), 0);
    EXPECT_EQ(pathToSubRecordInt.getStep(1).getField().getDiscriminator(), 0);

    auto result2 = babelwires::tryFollowPath(typeSystem, *recordType, pathToSubRecordInt, value);
    ASSERT_TRUE(result2.has_value());
    auto& [type2, vh2] = *result2;
    EXPECT_TRUE(vh2->tryAs<babelwires::IntValue>());
}
