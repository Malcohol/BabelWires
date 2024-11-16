#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Map/SumOfMaps/sumOfMapsType.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    class TestSumOfMapsType : public babelwires::SumOfMapsType {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestSumOfMapsType"), 1);
        TestSumOfMapsType()
            : babelwires::SumOfMapsType(
                  {babelwires::DefaultIntType::getThisType(),
                   babelwires::DefaultRationalType::getThisType()},
                  {babelwires::DefaultIntType::getThisType(), babelwires::StringType::getThisType()}, 1,
                  0) {}
    };
} // namespace

TEST(SumOfMapsTypeTest, sumOfMapsTypeConstructor) {
    testUtils::TestEnvironment testEnvironment;

    TestSumOfMapsType sumOfMapsType;

    EXPECT_EQ(sumOfMapsType.getSummands().size(), 2 * 2);

    EXPECT_EQ(sumOfMapsType.getSourceTypes().size(), 2);
    EXPECT_EQ(sumOfMapsType.getSourceTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumOfMapsType.getSourceTypes()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(sumOfMapsType.getTargetTypes().size(), 2);
    EXPECT_EQ(sumOfMapsType.getTargetTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumOfMapsType.getTargetTypes()[1], babelwires::StringType::getThisType());
    EXPECT_EQ(sumOfMapsType.getIndexOfDefaultSourceType(), 1);
    EXPECT_EQ(sumOfMapsType.getIndexOfDefaultTargetType(), 0);
}

TEST(SumOfMapsTypeTest, sumOfMapsTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    TestSumOfMapsType sumOfMapsType;

    babelwires::ValueHolder newValue = sumOfMapsType.createValue(testEnvironment.m_typeSystem);

    EXPECT_TRUE(newValue);
    EXPECT_TRUE(sumOfMapsType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    const auto* mapValue = newValue->as<babelwires::MapValue>();
    EXPECT_NE(mapValue, nullptr);

    EXPECT_EQ(mapValue->getSourceTypeRef(), babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(mapValue->getTargetTypeRef(), babelwires::DefaultIntType::getThisType());
}

TEST(SumOfMapsTypeTest, validValues) {
    testUtils::TestEnvironment testEnvironment;

    TestSumOfMapsType sumOfMapsType;

    babelwires::MapValue map0(testEnvironment.m_typeSystem, babelwires::DefaultIntType::getThisType(),
                              babelwires::DefaultIntType::getThisType(), babelwires::MapEntryData::Kind::All2Sm);
    babelwires::MapValue map1(testEnvironment.m_typeSystem, babelwires::DefaultIntType::getThisType(),
                              babelwires::StringType::getThisType(), babelwires::MapEntryData::Kind::All21);
    babelwires::MapValue map2(testEnvironment.m_typeSystem, babelwires::DefaultRationalType::getThisType(),
                              babelwires::DefaultIntType::getThisType(), babelwires::MapEntryData::Kind::All21);
    babelwires::MapValue map3(testEnvironment.m_typeSystem, babelwires::DefaultRationalType::getThisType(),
                              babelwires::StringType::getThisType(), babelwires::MapEntryData::Kind::All21);

    EXPECT_TRUE(sumOfMapsType.isValidValue(testEnvironment.m_typeSystem, map0));
    EXPECT_TRUE(sumOfMapsType.isValidValue(testEnvironment.m_typeSystem, map1));
    EXPECT_TRUE(sumOfMapsType.isValidValue(testEnvironment.m_typeSystem, map2));
    EXPECT_TRUE(sumOfMapsType.isValidValue(testEnvironment.m_typeSystem, map3));

    const auto [s0, t0] = sumOfMapsType.getIndexOfSourceAndTarget(sumOfMapsType.getIndexOfValue(testEnvironment.m_typeSystem, map0));
    const auto [s1, t1] = sumOfMapsType.getIndexOfSourceAndTarget(sumOfMapsType.getIndexOfValue(testEnvironment.m_typeSystem, map1));
    const auto [s2, t2] = sumOfMapsType.getIndexOfSourceAndTarget(sumOfMapsType.getIndexOfValue(testEnvironment.m_typeSystem, map2));
    const auto [s3, t3] = sumOfMapsType.getIndexOfSourceAndTarget(sumOfMapsType.getIndexOfValue(testEnvironment.m_typeSystem, map3));

    EXPECT_EQ(s0, 0);
    EXPECT_EQ(t0, 0);
    EXPECT_EQ(s1, 0);
    EXPECT_EQ(t1, 1);
    EXPECT_EQ(s2, 1);
    EXPECT_EQ(t2, 0);
    EXPECT_EQ(s3, 1);
    EXPECT_EQ(t3, 1);
}

