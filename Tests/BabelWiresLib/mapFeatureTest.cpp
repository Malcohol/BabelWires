#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapFeature.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(MapFeatureTest, construction) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::MapFeature> rootedFeature(testEnvironment.m_projectContext,
                                                                    babelwires::DefaultIntType::getThisIdentifier(),
                                                                    babelwires::DefaultIntType::getThisIdentifier());
    babelwires::MapFeature& mapFeature = rootedFeature.getFeature();

    mapFeature.setToDefault();
}

TEST(MapFeatureTest, setToDefault) {
    testUtils::TestEnvironment environment;

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);
    // The field identifier here doesn't need to be correctly registered.
    babelwires::MapFeature* mapFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                              testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo"));

    mapFeature->setToDefault();

    const babelwires::MapValue* const mapValue = mapFeature->getValue().as<babelwires::MapValue>();

    EXPECT_TRUE(mapValue->isValid(environment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);

    babelwires::MapFeature& mapFeature =
        *rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(), babelwires::DefaultIntType::getThisIdentifier()),
                              testUtils::getTestRegisteredIdentifier("foo"));
    babelwires::MapFeature& mapFeature2 =
        *rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(), babelwires::DefaultIntType::getThisIdentifier()),
                              testUtils::getTestRegisteredIdentifier("bar"));
    babelwires::IntFeature& intFeature = *rootFeature.addField(std::make_unique<babelwires::IntFeature>(),
                                                               testUtils::getTestRegisteredIdentifier("boo"));

    EXPECT_EQ(mapFeature.getKind(), mapFeature2.getKind());
    EXPECT_NE(mapFeature.getKind(), intFeature.getKind());
}

TEST(MapFeatureTest, assign) {
    testUtils::TestEnvironment environment;

    // MapFeatures expect to be able to find the typeSystem via the rootFeature at the root of the feature
    // hierarchy.
    babelwires::RootFeature rootFeature(environment.m_projectContext);

    // Note: The field identifiers here don't need to be correctly registered.
    babelwires::MapFeature* testTypeTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                              testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo1"));
    babelwires::MapFeature* testEnumTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                              testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo2"));
    babelwires::MapFeature* testTypeTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                              testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo3"));
    babelwires::MapFeature* testEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(),
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
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(
                                 testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo5"));
    babelwires::MapFeature* testSubEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(
                                 testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo6"));
    babelwires::MapFeature* testSubEnumTestSubEnumFeature = rootFeature.addField(
        std::make_unique<babelwires::MapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
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
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                              testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo1"));
    babelwires::MapFeature* testEnumTestTypeFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                              testUtils::TestType::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo2"));
    babelwires::MapFeature* testTypeTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestType::getThisIdentifier(),
                                                                              testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo3"));
    babelwires::MapFeature* testEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(testUtils::TestEnum::getThisIdentifier(),
                                                                              testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo4"));

    testTypeTestTypeFeature->setToDefault();
    testTypeTestEnumFeature->setToDefault();
    testEnumTestTypeFeature->setToDefault();
    testEnumTestEnumFeature->setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature->setValue(testTypeTestTypeFeature->getValue()));
    EXPECT_THROW(testTypeTestTypeFeature->setValue(testTypeTestEnumFeature->getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->setValue(testEnumTestTypeFeature->getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature->setValue(testEnumTestEnumFeature->getValue()), babelwires::ModelException);

    babelwires::MapFeature* testEnumTestSubEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(
                                 testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo5"));
    babelwires::MapFeature* testSubEnumTestEnumFeature =
        rootFeature.addField(std::make_unique<babelwires::MapFeature>(
                                 testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()),
                             testUtils::getTestRegisteredIdentifier("Foo6"));
    babelwires::MapFeature* testSubEnumTestSubEnumFeature = rootFeature.addField(
        std::make_unique<babelwires::MapFeature>(testUtils::TestSubEnum::getThisIdentifier(),
                                                         testUtils::TestSubEnum::getThisIdentifier()),
        testUtils::getTestRegisteredIdentifier("Foo7"));

    testEnumTestSubEnumFeature->setToDefault();
    testSubEnumTestEnumFeature->setToDefault();
    testSubEnumTestSubEnumFeature->setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnumFeature->setValue(testEnumTestSubEnumFeature->getValue()));
    testEnumTestEnumFeature->setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnumFeature->setValue(testEnumTestEnumFeature->getValue()), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnumFeature->setValue(testEnumTestEnumFeature->getValue()));
    testSubEnumTestEnumFeature->setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnumFeature->setValue(testSubEnumTestEnumFeature->getValue()));
    testEnumTestEnumFeature->setToDefault();

    // Source and target changing

    EXPECT_NO_THROW(testEnumTestEnumFeature->setValue(testSubEnumTestSubEnumFeature->getValue()));
    testEnumTestEnumFeature->setToDefault();

    EXPECT_THROW(testSubEnumTestSubEnumFeature->setValue(testEnumTestEnumFeature->getValue()), babelwires::ModelException);
}
