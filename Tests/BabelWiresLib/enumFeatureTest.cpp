#include <gtest/gtest.h>

#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(EnumFeatureTest, enumFeature) { 
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature enumFeature{testEnvironment.m_typeSystem, testUtils::TestEnum::getThisIdentifier()};
    babelwires::Instance<testUtils::TestEnum> enumInstance{enumFeature};
 
    enumFeature.setToDefault();
    EXPECT_EQ(enumInstance.get(), testUtils::TestEnum::Value::Bar);

    enumInstance.set(testUtils::TestEnum::Value::Boo);
    EXPECT_EQ(enumInstance.get(), testUtils::TestEnum::Value::Boo);

    babelwires::SimpleValueFeature enumFeature2{testEnvironment.m_typeSystem, testUtils::TestEnum::getThisIdentifier()};
    babelwires::Instance<testUtils::TestEnum> enumInstance2{enumFeature2};

    enumFeature2.setToDefault();
    enumFeature2.assign(enumFeature);
    EXPECT_EQ(enumInstance2.get(), testUtils::TestEnum::Value::Boo);
}

TEST(EnumFeatureTest, enumFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature enumFeature{testEnvironment.m_typeSystem, testUtils::TestEnum::getThisIdentifier()};
    babelwires::Instance<testUtils::TestEnum> enumInstance{enumFeature};
    
    // After construction, everything has changed.
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    enumFeature.clearChanges();
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    enumFeature.setToDefault();
    // Don't assume anything about the constructed value, so don't test for value changed.
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    enumInstance.set(testUtils::TestEnum::Value::Boo);
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    enumFeature.clearChanges();
    enumInstance.set(testUtils::TestEnum::Value::Boo);
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(EnumFeatureTest, enumFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature enumFeature{testEnvironment.m_typeSystem, testUtils::TestEnum::getThisIdentifier()};
    babelwires::Instance<testUtils::TestEnum> enumInstance{enumFeature};

    enumInstance.set(testUtils::TestEnum::Value::Boo);
    const std::size_t hashAt0 = enumFeature.getHash();

    enumInstance.set(testUtils::TestEnum::Value::Bar);
    const std::size_t hashAt2 = enumFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt2);
}
