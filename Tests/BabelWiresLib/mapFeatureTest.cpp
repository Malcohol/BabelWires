#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    // Describes an IntType -> IntType map.
    template<typename SOURCE_TYPE, typename TARGET_TYPE>
    babelwires::TypeRef getTestMapTypeRef() {
        return babelwires::TypeRef(
            babelwires::MapTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {SOURCE_TYPE::getThisIdentifier(), TARGET_TYPE::getThisIdentifier()},
                {babelwires::EnumValue(babelwires::MapEntryFallbackKind::getIdentifierFromValue(
                    babelwires::MapEntryFallbackKind::Value::All21))}});
    }
} // namespace

TEST(MapFeatureTest, construction) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature mapFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<babelwires::DefaultIntType, babelwires::DefaultIntType>());

    mapFeature.setToDefault();
}

TEST(MapFeatureTest, setToDefault) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature mapFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, testUtils::TestType>());

    mapFeature.setToDefault();

    const babelwires::MapValue* const mapValue = mapFeature.getValue()->as<babelwires::MapValue>();

    EXPECT_TRUE(mapValue->isValid(testEnvironment.m_typeSystem));
}

TEST(MapFeatureTest, isCompatible) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);

    babelwires::SimpleValueFeature mapFeature1(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, babelwires::DefaultIntType>());
    babelwires::SimpleValueFeature mapFeature2(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, babelwires::DefaultIntType>());
    babelwires::SimpleValueFeature intFeature(testEnvironment.m_typeSystem, babelwires::DefaultIntType::getThisIdentifier());

    EXPECT_EQ(mapFeature1.getKind(), mapFeature2.getKind());
    EXPECT_NE(mapFeature1.getKind(), intFeature.getKind());
}

TEST(MapFeatureTest, assign) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature testTypeTestTypeFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, testUtils::TestType>());
    babelwires::SimpleValueFeature testEnumTestTypeFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestType>());
    babelwires::SimpleValueFeature testTypeTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, testUtils::TestEnum>());
    babelwires::SimpleValueFeature testEnumTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestEnum>());

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature.assign(testTypeTestTypeFeature));
    EXPECT_THROW(testTypeTestTypeFeature.assign(testTypeTestEnumFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.assign(testEnumTestTypeFeature), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.assign(testEnumTestEnumFeature), babelwires::ModelException);

    babelwires::SimpleValueFeature testEnumTestSubEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestSubEnum>());
    babelwires::SimpleValueFeature testSubEnumTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestSubEnum, testUtils::TestEnum>());
    babelwires::SimpleValueFeature testSubEnumTestSubEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestSubEnum, testUtils::TestSubEnum>());

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

    babelwires::SimpleValueFeature testTypeTestTypeFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, testUtils::TestType>());
    babelwires::SimpleValueFeature testEnumTestTypeFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestType>());
    babelwires::SimpleValueFeature testTypeTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestType, testUtils::TestEnum>());
    babelwires::SimpleValueFeature testEnumTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestEnum>());

    testTypeTestTypeFeature.setToDefault();
    testTypeTestEnumFeature.setToDefault();
    testEnumTestTypeFeature.setToDefault();
    testEnumTestEnumFeature.setToDefault();

    EXPECT_NO_THROW(testTypeTestTypeFeature.setValue(testTypeTestTypeFeature.getValue()));
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testTypeTestEnumFeature.getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testEnumTestTypeFeature.getValue()), babelwires::ModelException);
    EXPECT_THROW(testTypeTestTypeFeature.setValue(testEnumTestEnumFeature.getValue()), babelwires::ModelException);

    babelwires::SimpleValueFeature testEnumTestSubEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestEnum, testUtils::TestSubEnum>());
    babelwires::SimpleValueFeature testSubEnumTestEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestSubEnum, testUtils::TestEnum>());
    babelwires::SimpleValueFeature testSubEnumTestSubEnumFeature(testEnvironment.m_typeSystem, getTestMapTypeRef<testUtils::TestSubEnum, testUtils::TestSubEnum>());

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
