#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    class TestSumType : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestSumType"), 1);
        TestSumType(unsigned int defaultType)
            : babelwires::SumType({babelwires::DefaultIntType::getThisIdentifier(),
                                   babelwires::DefaultRationalType::getThisIdentifier()},
                                  defaultType) {}
    };
} // namespace

TEST(SumTypeTest, sumTypeDefault0) {
    testUtils::TestEnvironment testEnvironment;

    TestSumType sumType(0);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 0);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->as<babelwires::IntValue>(), nullptr);

    babelwires::ValueHolder rationalValue =
        testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultRationalType::getThisIdentifier()).createValue(
            testEnvironment.m_typeSystem);

    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *rationalValue));
}

TEST(SumTypeTest, sumTypeDefault1) {
    testUtils::TestEnvironment testEnvironment;

    TestSumType sumType(1);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 1);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->as<babelwires::RationalValue>(), nullptr);

    babelwires::ValueHolder intValue =
        testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultIntType::getThisIdentifier()).createValue(
            testEnvironment.m_typeSystem);

    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *intValue));
}
