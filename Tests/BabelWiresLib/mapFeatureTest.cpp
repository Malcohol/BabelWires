#include <gtest/gtest.h>

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/testLog.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

namespace {
    const babelwires::LongIdentifier testTypeId1 = "TestType1";
    const babelwires::LongIdentifier testTypeId2 = "TestType2";
} // namespace

TEST(MapFeatureTest, construction)
{
    babelwires::MapFeature mapFeature(testTypeId1, testTypeId2);

    EXPECT_EQ(mapFeature.getSourceTypeId(), testTypeId1);
    EXPECT_EQ(mapFeature.getTargetTypeId(), testTypeId2);
}

TEST(MapFeatureTest, setToDefault) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);
    // The field identifier here doesn't need to be correctly registered.
    babelwires::MapFeature* mapFeature = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo"));

    mapFeature->setToDefault();

    const babelwires::MapData& mapData = mapFeature->get();
    
    EXPECT_TRUE(mapData.isValid(environment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    babelwires::MapFeature mapFeature(testTypeId1, testTypeId2);
    babelwires::MapFeature mapFeature2(testTypeId1, testTypeId2);
    babelwires::IntFeature intFeature;

    EXPECT_TRUE(mapFeature.isCompatible(mapFeature2));
    EXPECT_FALSE(mapFeature.isCompatible(intFeature));
    EXPECT_FALSE(intFeature.isCompatible(mapFeature));
}

TEST(MapFeatureTest, assign) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);
    // The field identifier here doesn't need to be correctly registered.
    babelwires::MapFeature* testTypeTestType = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo1"));
    babelwires::MapFeature* testEnumTestType = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(), testUtils::TestType::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo2"));
    babelwires::MapFeature* testTypeTestEnum = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo3"));
    babelwires::MapFeature* testEnumTestEnum = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo4"));

    testTypeTestType->setToDefault();
    testTypeTestEnum->setToDefault();
    testEnumTestType->setToDefault();
    testEnumTestEnum->setToDefault();

    EXPECT_NO_THROW(testTypeTestType->assign(*testTypeTestType));
    EXPECT_THROW(testTypeTestType->assign(*testTypeTestEnum), babelwires::ModelException);
    EXPECT_THROW(testTypeTestType->assign(*testEnumTestType), babelwires::ModelException);
    EXPECT_THROW(testTypeTestType->assign(*testEnumTestEnum), babelwires::ModelException);

    babelwires::MapFeature* testEnumTestSubEnum = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo5"));
    babelwires::MapFeature* testSubEnumTestEnum = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo6"));
    babelwires::MapFeature* testSubEnumTestSubEnum = rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()),
            testUtils::getTestRegisteredIdentifier("Foo7"));

    testEnumTestSubEnum->setToDefault();
    testSubEnumTestEnum->setToDefault();
    testSubEnumTestSubEnum->setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnum->assign(*testEnumTestSubEnum));
    testEnumTestEnum->setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnum->assign(*testEnumTestEnum), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnum->assign(*testEnumTestEnum));
    testSubEnumTestEnum->setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnum->assign(*testSubEnumTestEnum));
    testEnumTestEnum->setToDefault();
}
