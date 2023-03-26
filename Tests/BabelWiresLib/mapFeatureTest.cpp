#include <gtest/gtest.h>

#include <BabelWiresLib/Features/mapFeature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::PrimitiveTypeId testTypeId1 = "TestType1";
    const babelwires::PrimitiveTypeId testTypeId2 = "TestType2";
} // namespace

TEST(MapFeatureTest, construction) {
    babelwires::StandardMapFeature mapFeature(testTypeId1, testTypeId2);

    babelwires::MapFeature::AllowedTypes allowedTypes;
    mapFeature.getAllowedSourceTypeRefs(allowedTypes);
    EXPECT_TRUE(testUtils::areEqualSets(allowedTypes.m_typeRefs, std::vector<babelwires::TypeRef>{testTypeId1}));
    mapFeature.getAllowedTargetTypeRefs(allowedTypes);
    EXPECT_TRUE(testUtils::areEqualSets(allowedTypes.m_typeRefs, std::vector<babelwires::TypeRef>{testTypeId2}));
}

TEST(MapFeatureTest, setToDefault) {
    testUtils::TestEnvironment environment;
    
    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);
    // The field identifier here doesn't need to be correctly registered.
    babelwires::MapFeature* mapFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                      testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo"));

    mapFeature->setToDefault();

    const babelwires::MapData& mapData = mapFeature->get();

    EXPECT_TRUE(mapData.isValid(environment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    babelwires::StandardMapFeature mapFeature(testTypeId1, testTypeId2);
    babelwires::StandardMapFeature mapFeature2(testTypeId1, testTypeId2);
    babelwires::IntFeature intFeature;

    EXPECT_TRUE(mapFeature.isCompatible(mapFeature2));
    EXPECT_FALSE(mapFeature.isCompatible(intFeature));
    EXPECT_FALSE(intFeature.isCompatible(mapFeature));
}

TEST(MapFeatureTest, assign) {
    testUtils::TestEnvironment environment;

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature
    // hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);

    // Note: The field identifiers here don't need to be correctly registered.
    babelwires::MapFeature* testTypeTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                      testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo1"));
    babelwires::MapFeature* testEnumTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo2"));
    babelwires::MapFeature* testTypeTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo3"));
    babelwires::MapFeature* testEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo4"));

    testTypeTestTypeFeature->setToDefault();
    testTypeTestEnumFeature->setToDefault();
    testEnumTestTypeFeature->setToDefault();
    testEnumTestEnumFeature->setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature->assign(*testTypeTestTypeFeature));
    EXPECT_THROW(testTypeTestTypeFeature->assign(*testTypeTestEnumFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->assign(*testEnumTestTypeFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->assign(*testEnumTestEnumFeature), babelwires::ModelException);

    babelwires::MapFeature* testEnumTestSubEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo5"));
    babelwires::MapFeature* testSubEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo6"));
    babelwires::MapFeature* testSubEnumTestSubEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
                                                                      testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo7"));

    testEnumTestSubEnumFeature->setToDefault();
    testSubEnumTestEnumFeature->setToDefault();
    testSubEnumTestSubEnumFeature->setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnumFeature->assign(*testEnumTestSubEnumFeature));
    testEnumTestEnumFeature->setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnumFeature->assign(*testEnumTestEnumFeature), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnumFeature->assign(*testEnumTestEnumFeature));
    testSubEnumTestEnumFeature->setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnumFeature->assign(*testSubEnumTestEnumFeature));
    testEnumTestEnumFeature->setToDefault();

    // Source and target changing

    EXPECT_NO_THROW(testEnumTestEnumFeature->assign(*testSubEnumTestSubEnumFeature));
    testEnumTestEnumFeature->setToDefault();

    EXPECT_THROW(testSubEnumTestSubEnumFeature->assign(*testEnumTestEnumFeature), babelwires::ModelException);
}

TEST(MapFeatureTest, setAndGet) {
    testUtils::TestEnvironment environment;

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature
    // hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);

    // Note: The field identifiers here don't need to be correctly registered.
    babelwires::MapFeature* testTypeTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                      testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo1"));
    babelwires::MapFeature* testEnumTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo2"));
    babelwires::MapFeature* testTypeTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo3"));
    babelwires::MapFeature* testEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo4"));

    testTypeTestTypeFeature->setToDefault();
    testTypeTestEnumFeature->setToDefault();
    testEnumTestTypeFeature->setToDefault();
    testEnumTestEnumFeature->setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature->set(testTypeTestTypeFeature->get()));
    EXPECT_THROW(testTypeTestTypeFeature->set(testTypeTestEnumFeature->get()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->set(testEnumTestTypeFeature->get()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->set(testEnumTestEnumFeature->get()), babelwires::ModelException);

    babelwires::MapFeature* testEnumTestSubEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                      testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo5"));
    babelwires::MapFeature* testSubEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo6"));
    babelwires::MapFeature* testSubEnumTestSubEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::StandardMapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
                                                                      testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo7"));

    testEnumTestSubEnumFeature->setToDefault();
    testSubEnumTestEnumFeature->setToDefault();
    testSubEnumTestSubEnumFeature->setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnumFeature->set(testEnumTestSubEnumFeature->get()));
    testEnumTestEnumFeature->setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnumFeature->set(testEnumTestEnumFeature->get()), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnumFeature->set(testEnumTestEnumFeature->get()));
    testSubEnumTestEnumFeature->setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnumFeature->set(testSubEnumTestEnumFeature->get()));
    testEnumTestEnumFeature->setToDefault();

    // Source and target changing

    EXPECT_NO_THROW(testEnumTestEnumFeature->set(testSubEnumTestSubEnumFeature->get()));
    testEnumTestEnumFeature->setToDefault();

    EXPECT_THROW(testSubEnumTestSubEnumFeature->set(testEnumTestEnumFeature->get()), babelwires::ModelException);
}
