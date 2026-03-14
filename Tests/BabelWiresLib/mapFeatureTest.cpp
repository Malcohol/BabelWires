#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    // Describes an IntType -> IntType map.
    template <typename SOURCE_TYPE, typename TARGET_TYPE> babelwires::TypePtr getTestMapType(const babelwires::TypeSystem& typeSystem) {
        return babelwires::MapTypeConstructor::makeTypeExp(SOURCE_TYPE::getThisIdentifier(), TARGET_TYPE::getThisIdentifier()).assertResolve(typeSystem);
    }
} // namespace

TEST(MapFeatureTest, construction) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature(
        testEnvironment.m_typeSystem, getTestMapType<babelwires::DefaultIntType, babelwires::DefaultIntType>(testEnvironment.m_typeSystem));

    mapFeature.setToDefault();
}

TEST(MapFeatureTest, setToDefault) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature(testEnvironment.m_typeSystem,
                                              getTestMapType<babelwires::StringType, babelwires::StringType>(testEnvironment.m_typeSystem));

    mapFeature.setToDefault();

    const babelwires::MapValue* const mapValue = mapFeature.getValue()->tryAs<babelwires::MapValue>();

    EXPECT_TRUE(mapValue->isValid(testEnvironment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature1(testEnvironment.m_typeSystem,
                                               getTestMapType<babelwires::StringType, babelwires::DefaultIntType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot mapFeature2(testEnvironment.m_typeSystem,
                                               getTestMapType<babelwires::StringType, babelwires::DefaultIntType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot intFeature(testEnvironment.m_typeSystem,
                                              testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultIntType>());

    EXPECT_EQ(mapFeature1.getFlavour(), mapFeature2.getFlavour());
    EXPECT_NE(mapFeature1.getFlavour(), intFeature.getFlavour());
}

TEST(MapFeatureTest, assign) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testTypeTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapType<babelwires::StringType, babelwires::StringType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testEnumTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, babelwires::StringType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testTypeTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<babelwires::StringType, testDomain::TestEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, testDomain::TestEnum>(testEnvironment.m_typeSystem));

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_TRUE(testTypeTestTypeFeature.assign(testTypeTestTypeFeature));
    EXPECT_FALSE(testTypeTestTypeFeature.assign(testTypeTestEnumFeature));
    EXPECT_FALSE(testTypeTestTypeFeature.assign(testEnumTestTypeFeature));
    EXPECT_FALSE(testTypeTestTypeFeature.assign(testEnumTestEnumFeature));

    babelwires::ValueTreeRoot testEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, testDomain::TestSubEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testSubEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestSubEnum, testDomain::TestEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testSubEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestSubEnum, testDomain::TestSubEnum>(testEnvironment.m_typeSystem));

    testEnumTestSubEnumFeature.setToDefault();
    testSubEnumTestEnumFeature.setToDefault();
    testSubEnumTestSubEnumFeature.setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_TRUE(testEnumTestEnumFeature.assign(testEnumTestSubEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_FALSE(testEnumTestSubEnumFeature.assign(testEnumTestEnumFeature));

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_TRUE(testSubEnumTestEnumFeature.assign(testEnumTestEnumFeature));
    testSubEnumTestEnumFeature.setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_TRUE(testEnumTestEnumFeature.assign(testSubEnumTestEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    // Source and target changing

    EXPECT_TRUE(testEnumTestEnumFeature.assign(testSubEnumTestSubEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    EXPECT_FALSE(testSubEnumTestSubEnumFeature.assign(testEnumTestEnumFeature));
}

TEST(MapFeatureTest, setAndGet) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testTypeTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapType<babelwires::StringType, babelwires::StringType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testEnumTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, babelwires::StringType>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testTypeTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<babelwires::StringType, testDomain::TestEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, testDomain::TestEnum>(testEnvironment.m_typeSystem));

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_TRUE(testTypeTestTypeFeature.setValue(testTypeTestTypeFeature.getValue()));
    EXPECT_FALSE(testTypeTestTypeFeature.setValue(testTypeTestEnumFeature.getValue()));
    EXPECT_FALSE(testTypeTestTypeFeature.setValue(testEnumTestTypeFeature.getValue()));
    EXPECT_FALSE(testTypeTestTypeFeature.setValue(testEnumTestEnumFeature.getValue()));

    babelwires::ValueTreeRoot testEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestEnum, testDomain::TestSubEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testSubEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestSubEnum, testDomain::TestEnum>(testEnvironment.m_typeSystem));
    babelwires::ValueTreeRoot testSubEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapType<testDomain::TestSubEnum, testDomain::TestSubEnum>(testEnvironment.m_typeSystem));

    testEnumTestSubEnumFeature.setToDefault();
    testSubEnumTestEnumFeature.setToDefault();
    testSubEnumTestSubEnumFeature.setToDefault();
    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_TRUE(testEnumTestEnumFeature.setValue(testEnumTestSubEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_FALSE(testEnumTestSubEnumFeature.setValue(testEnumTestEnumFeature.getValue()));

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_TRUE(testSubEnumTestEnumFeature.setValue(testEnumTestEnumFeature.getValue()));
    testSubEnumTestEnumFeature.setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_TRUE(testEnumTestEnumFeature.setValue(testSubEnumTestEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    // Source and target changing

    EXPECT_TRUE(testEnumTestEnumFeature.setValue(testSubEnumTestSubEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    EXPECT_FALSE(testSubEnumTestSubEnumFeature.setValue(testEnumTestEnumFeature.getValue()));
}
