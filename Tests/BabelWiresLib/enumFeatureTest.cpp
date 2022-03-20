#include <gtest/gtest.h>

#include "BabelWiresLib/Features/rootFeature.hpp"

#include "Tests/BabelWiresLib/TestUtils/testEnum.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"

#include "Tests/TestUtils/testIdentifiers.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"

TEST(FeatureTest, enumFeature) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::EnumFeature& enumFeature = *rootFeature.addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()), testUtils::getTestRegisteredIdentifier("aaa"));

    const babelwires::Enum::EnumValues enumValues = testUtils::getTestEnumValues();

    enumFeature.setToDefault();
    EXPECT_EQ(enumFeature.get(), enumValues[1]);

    enumFeature.set(enumValues[0]);
    EXPECT_EQ(enumFeature.get(), enumValues[0]);

    babelwires::EnumFeature& enumFeature2 = *rootFeature.addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()), testUtils::getTestRegisteredIdentifier("bbb"));
    enumFeature2.setToDefault();
    enumFeature2.assign(enumFeature);
    EXPECT_EQ(enumFeature2.get(), enumValues[0]);
}

TEST(FeatureTest, enumFeatureChanges) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::EnumFeature& enumFeature = *rootFeature.addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()), testUtils::getTestRegisteredIdentifier("flerm"));

    const babelwires::Enum::EnumValues enumValues = testUtils::getTestEnumValues();

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

    enumFeature.set(enumValues[0]);
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    enumFeature.clearChanges();
    enumFeature.set(enumValues[0]);
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(enumFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(FeatureTest, enumFeatureHash) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::EnumFeature& enumFeature = *rootFeature.addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()), testUtils::getTestRegisteredIdentifier("flerm"));

    const babelwires::Enum::EnumValues enumValues = testUtils::getTestEnumValues();

    enumFeature.set(enumValues[0]);
    const std::size_t hashAt0 = enumFeature.getHash();

    enumFeature.set(enumValues[2]);
    const std::size_t hashAt2 = enumFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt2);
}
