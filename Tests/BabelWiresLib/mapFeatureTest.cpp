#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    // Describes an IntType -> IntType map.
    template <typename SOURCE_TYPE, typename TARGET_TYPE> babelwires::TypeRef getTestMapTypeRef() {
        return babelwires::MapTypeConstructor::makeTypeRef(SOURCE_TYPE::getThisType(), TARGET_TYPE::getThisType());
    }
} // namespace

TEST(MapFeatureTest, construction) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::DefaultIntType, babelwires::DefaultIntType>());

    mapFeature.setToDefault();
}

TEST(MapFeatureTest, setToDefault) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature(testEnvironment.m_typeSystem,
                                              getTestMapTypeRef<babelwires::StringType, babelwires::StringType>());

    mapFeature.setToDefault();

    const babelwires::MapValue* const mapValue = mapFeature.getValue()->as<babelwires::MapValue>();

    EXPECT_TRUE(mapValue->isValid(testEnvironment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot mapFeature1(testEnvironment.m_typeSystem,
                                               getTestMapTypeRef<babelwires::StringType, babelwires::DefaultIntType>());
    babelwires::ValueTreeRoot mapFeature2(testEnvironment.m_typeSystem,
                                               getTestMapTypeRef<babelwires::StringType, babelwires::DefaultIntType>());
    babelwires::ValueTreeRoot intFeature(testEnvironment.m_typeSystem,
                                              babelwires::DefaultIntType::getThisType());

    EXPECT_EQ(mapFeature1.getFlavour(), mapFeature2.getFlavour());
    EXPECT_NE(mapFeature1.getFlavour(), intFeature.getFlavour());
}

TEST(MapFeatureTest, assign) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testTypeTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::StringType, babelwires::StringType>());
    babelwires::ValueTreeRoot testEnumTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, babelwires::StringType>());
    babelwires::ValueTreeRoot testTypeTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::StringType, testDomain::TestEnum>());
    babelwires::ValueTreeRoot testEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, testDomain::TestEnum>());

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature.assign(testTypeTestTypeFeature));
    EXPECT_THROW(testTypeTestTypeFeature.assign(testTypeTestEnumFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.assign(testEnumTestTypeFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.assign(testEnumTestEnumFeature), babelwires::ModelException);

    babelwires::ValueTreeRoot testEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, testDomain::TestSubEnum>());
    babelwires::ValueTreeRoot testSubEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestSubEnum, testDomain::TestEnum>());
    babelwires::ValueTreeRoot testSubEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestSubEnum, testDomain::TestSubEnum>());

    testEnumTestSubEnumFeature.setToDefault();
    testSubEnumTestEnumFeature.setToDefault();
    testSubEnumTestSubEnumFeature.setToDefault();

    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnumFeature.assign(testEnumTestSubEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnumFeature.assign(testEnumTestEnumFeature), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnumFeature.assign(testEnumTestEnumFeature));
    testSubEnumTestEnumFeature.setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnumFeature.assign(testSubEnumTestEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    // Source and target changing

    EXPECT_NO_THROW(testEnumTestEnumFeature.assign(testSubEnumTestSubEnumFeature));
    testEnumTestEnumFeature.setToDefault();

    EXPECT_THROW(testSubEnumTestSubEnumFeature.assign(testEnumTestEnumFeature), babelwires::ModelException);
}

TEST(MapFeatureTest, setAndGet) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot testTypeTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::StringType, babelwires::StringType>());
    babelwires::ValueTreeRoot testEnumTestTypeFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, babelwires::StringType>());
    babelwires::ValueTreeRoot testTypeTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::StringType, testDomain::TestEnum>());
    babelwires::ValueTreeRoot testEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, testDomain::TestEnum>());

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature.setValue(testTypeTestTypeFeature.getValue()));
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testTypeTestEnumFeature.getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testEnumTestTypeFeature.getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testEnumTestEnumFeature.getValue()), babelwires::ModelException);

    babelwires::ValueTreeRoot testEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestEnum, testDomain::TestSubEnum>());
    babelwires::ValueTreeRoot testSubEnumTestEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestSubEnum, testDomain::TestEnum>());
    babelwires::ValueTreeRoot testSubEnumTestSubEnumFeature(
        testEnvironment.m_typeSystem, getTestMapTypeRef<testDomain::TestSubEnum, testDomain::TestSubEnum>());

    testEnumTestSubEnumFeature.setToDefault();
    testSubEnumTestEnumFeature.setToDefault();
    testSubEnumTestSubEnumFeature.setToDefault();
    // Target types are handled covariantly:

    // All target values in the new map are values in the original target type.
    EXPECT_NO_THROW(testEnumTestEnumFeature.setValue(testEnumTestSubEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    // A target value in the new map could be outside the range of the original target type.
    EXPECT_THROW(testEnumTestSubEnumFeature.setValue(testEnumTestEnumFeature.getValue()), babelwires::ModelException);

    // Because of fallbacks, any related (sub- or supertype) type are permitted.

    // The new map defines values for the whole of the original source type.
    EXPECT_NO_THROW(testSubEnumTestEnumFeature.setValue(testEnumTestEnumFeature.getValue()));
    testSubEnumTestEnumFeature.setToDefault();

    // The new map defines values for the whole of the original source type (via the fallback).
    EXPECT_NO_THROW(testEnumTestEnumFeature.setValue(testSubEnumTestEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    // Source and target changing

    EXPECT_NO_THROW(testEnumTestEnumFeature.setValue(testSubEnumTestSubEnumFeature.getValue()));
    testEnumTestEnumFeature.setToDefault();

    EXPECT_THROW(testSubEnumTestSubEnumFeature.setValue(testEnumTestEnumFeature.getValue()),
                 babelwires::ModelException);
}
