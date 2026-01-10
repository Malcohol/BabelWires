#include <gtest/gtest.h>

#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(EnumFeatureTest, enumFeature) { 
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot enumFeature{testEnvironment.m_typeSystem, testDomain::TestEnum::getThisIdentifier()};
    babelwires::Instance<testDomain::TestEnum> enumInstance{enumFeature};
 
    enumFeature.setToDefault();
    EXPECT_EQ(enumInstance.get(), testDomain::TestEnum::Value::Bar);

    enumInstance.set(testDomain::TestEnum::Value::Boo);
    EXPECT_EQ(enumInstance.get(), testDomain::TestEnum::Value::Boo);

    babelwires::ValueTreeRoot enumFeature2{testEnvironment.m_typeSystem, testDomain::TestEnum::getThisIdentifier()};
    babelwires::Instance<testDomain::TestEnum> enumInstance2{enumFeature2};

    enumFeature2.setToDefault();
    enumFeature2.assign(enumFeature);
    EXPECT_EQ(enumInstance2.get(), testDomain::TestEnum::Value::Boo);
}

TEST(EnumFeatureTest, enumFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot enumFeature{testEnvironment.m_typeSystem, testDomain::TestEnum::getThisIdentifier()};
    babelwires::Instance<testDomain::TestEnum> enumInstance{enumFeature};
    
    // After construction, everything has changed.
    EXPECT_TRUE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));

    enumFeature.clearChanges();
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));

    enumFeature.setToDefault();
    // Don't assume anything about the constructed value, so don't test for value changed.
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));

    enumInstance.set(testDomain::TestEnum::Value::Boo);
    EXPECT_TRUE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));

    enumFeature.clearChanges();
    enumInstance.set(testDomain::TestEnum::Value::Boo);
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
}

TEST(EnumFeatureTest, enumFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot enumFeature{testEnvironment.m_typeSystem, testDomain::TestEnum::getThisIdentifier()};
    babelwires::Instance<testDomain::TestEnum> enumInstance{enumFeature};

    enumInstance.set(testDomain::TestEnum::Value::Boo);
    const std::size_t hashAt0 = enumFeature.getHash();

    enumInstance.set(testDomain::TestEnum::Value::Bar);
    const std::size_t hashAt2 = enumFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt2);
}
