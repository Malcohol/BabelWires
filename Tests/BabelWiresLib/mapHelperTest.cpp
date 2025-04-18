#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Helpers/enumSourceMapApplicator.hpp>
#include <BabelWiresLib/Types/Map/Helpers/enumValueAdapters.hpp>
#include <BabelWiresLib/Types/Map/Helpers/unorderedMapApplicator.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

namespace {
    std::string testValueAdapter(const babelwires::Value& value) {
        return value.as<testUtils::TestValue>()->m_value;
    }

    babelwires::MapValue setUpTestMapValue(const babelwires::TypeSystem& typeSystem,
                                         const babelwires::TypeRef& sourceTypeId,
                                         const babelwires::TypeRef& targetTypeId, const babelwires::EditableValue& sourceValue1,
                                         const babelwires::EditableValue& sourceValue2, const babelwires::EditableValue& targetValue1,
                                         const babelwires::EditableValue& targetValue2, const babelwires::EditableValue& targetValue3,
                                         bool allToOneFallback) {

        auto oneToOne1 = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, sourceTypeId, targetTypeId);
        oneToOne1->setSourceValue(sourceValue1.clone());
        oneToOne1->setTargetValue(targetValue1.clone());

        auto oneToOne2 = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, sourceTypeId, targetTypeId);
        oneToOne2->setSourceValue(sourceValue2.clone());
        oneToOne2->setTargetValue(targetValue2.clone());

        std::unique_ptr<babelwires::FallbackMapEntryData> fallback;
        if (allToOneFallback) {
            auto allToOne = std::make_unique<babelwires::AllToOneFallbackMapEntryData>(typeSystem, targetTypeId);
            allToOne->setTargetValue(targetValue3.clone());
            fallback = std::unique_ptr<babelwires::FallbackMapEntryData>(allToOne.release());
        } else {
            fallback = std::make_unique<babelwires::AllToSameFallbackMapEntryData>();
        }

        babelwires::MapValue mapValue;
        mapValue.setSourceTypeRef(sourceTypeId);
        mapValue.setTargetTypeRef(targetTypeId);
        mapValue.emplaceBack(std::move(oneToOne1));
        mapValue.emplaceBack(std::move(oneToOne2));
        mapValue.emplaceBack(std::move(fallback));
        return mapValue;
    }

    babelwires::MapValue setUpTestTypeMapValue(const babelwires::TypeSystem& typeSystem, babelwires::MapValue& mapValue,
                                             bool allToOneFallback) {
        testUtils::TestValue sourceValue1;
        sourceValue1.m_value = "aaa";

        testUtils::TestValue sourceValue2;
        sourceValue2.m_value = "bbb";

        testUtils::TestValue targetValue1;
        targetValue1.m_value = "xxx";

        testUtils::TestValue targetValue2;
        targetValue2.m_value = "yyy";

        testUtils::TestValue targetValue3;
        targetValue3.m_value = "zzz";

        return setUpTestMapValue(typeSystem, testUtils::TestType::getThisType(),
                                testUtils::TestType::getThisType(), sourceValue1, sourceValue2, targetValue1,
                                targetValue2, targetValue3, allToOneFallback);
    }

    babelwires::MapValue setUpTestEnumMapValue(const babelwires::TypeSystem& typeSystem, babelwires::MapValue& mapValue,
                                             bool allToOneFallback) {
        babelwires::EnumValue sourceValue1;
        sourceValue1.set("Foo");

        babelwires::EnumValue sourceValue2;
        sourceValue2.set("Bar");

        babelwires::EnumValue targetValue1;
        targetValue1.set("Oom");

        babelwires::EnumValue targetValue2;
        targetValue2.set("Boo");

        babelwires::EnumValue targetValue3;
        targetValue3.set("Erm");

        return setUpTestMapValue(typeSystem, testDomain::TestEnum::getThisType(),
                                testDomain::TestEnum::getThisType(), sourceValue1, sourceValue2, targetValue1,
                                targetValue2, targetValue3, allToOneFallback);
    }

    babelwires::MapValue setUpTestTypeTestEnumMapValue(const babelwires::TypeSystem& typeSystem,
                                                     babelwires::MapValue& mapValue) {
        testUtils::TestValue sourceValue1;
        sourceValue1.m_value = "aaa";

        testUtils::TestValue sourceValue2;
        sourceValue2.m_value = "bbb";

        babelwires::EnumValue targetValue1;
        targetValue1.set("Oom");

        babelwires::EnumValue targetValue2;
        targetValue2.set("Boo");

        babelwires::EnumValue targetValue3;
        targetValue3.set("Erm");

        return setUpTestMapValue(typeSystem, testUtils::TestType::getThisType(),
                                testDomain::TestEnum::getThisType(), sourceValue1, sourceValue2, targetValue1,
                                targetValue2, targetValue3, true);
    }

    babelwires::MapValue setUpTestEnumTestTypeMapValue(const babelwires::TypeSystem& typeSystem,
                                                     babelwires::MapValue& mapValue) {
        babelwires::EnumValue sourceValue1;
        sourceValue1.set("Foo");

        babelwires::EnumValue sourceValue2;
        sourceValue2.set("Bar");

        testUtils::TestValue targetValue1;
        targetValue1.m_value = "xxx";

        testUtils::TestValue targetValue2;
        targetValue2.m_value = "yyy";

        testUtils::TestValue targetValue3;
        targetValue3.m_value = "zzz";

        return setUpTestMapValue(typeSystem, testDomain::TestEnum::getThisType(),
                                testDomain::TestEnum::getThisType(), sourceValue1, sourceValue2, targetValue1,
                                targetValue2, targetValue3, true);
    }
} // namespace

TEST(MapHelperTest, unorderedMapApplicator_allToOneFallback) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapValue mapValue = setUpTestTypeMapValue(typeSystem, mapValue, true);

    babelwires::UnorderedMapApplicator<std::string, std::string> mapApplicator(mapValue, &testValueAdapter,
                                                                               &testValueAdapter);

    EXPECT_EQ(mapApplicator["aaa"], "xxx");
    EXPECT_EQ(mapApplicator["bbb"], "yyy");
    EXPECT_EQ(mapApplicator["ccc"], "zzz");
    EXPECT_EQ(mapApplicator["ddd"], "zzz");
}

TEST(MapHelperTest, unorderedMapApplicator_allToSameFallback) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapValue mapValue = setUpTestTypeMapValue(typeSystem, mapValue, false);

    babelwires::UnorderedMapApplicator<std::string, std::string> mapApplicator(mapValue, &testValueAdapter,
                                                                               &testValueAdapter);

    EXPECT_EQ(mapApplicator["aaa"], "xxx");
    EXPECT_EQ(mapApplicator["bbb"], "yyy");
    EXPECT_EQ(mapApplicator["ccc"], "ccc");
    EXPECT_EQ(mapApplicator["ddd"], "ddd");
}

TEST(MapHelperTest, unorderedMapApplicator_differentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    const testDomain::TestEnum& testEnum = typeSystem.getEntryByType<testDomain::TestEnum>();

    babelwires::MapValue mapValue = setUpTestTypeTestEnumMapValue(typeSystem, mapValue);

    // Also test the EnumToIndexValueAdapter
    babelwires::UnorderedMapApplicator<std::string, unsigned int> mapApplicator(
        mapValue, &testValueAdapter, babelwires::EnumToIndexValueAdapter{testEnum});

    EXPECT_EQ(mapApplicator["aaa"], 3);
    EXPECT_EQ(mapApplicator["bbb"], 4);
    EXPECT_EQ(mapApplicator["ccc"], 2);
    EXPECT_EQ(mapApplicator["ddd"], 2);
}

TEST(MapHelperTest, enumSourceMapApplicator_allToOneFallback) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testDomain::TestEnum>();

    const testDomain::TestEnum& testEnum = typeSystem.getEntryByType<testDomain::TestEnum>();

    babelwires::MapValue mapValue = setUpTestEnumMapValue(typeSystem, mapValue, true);

    babelwires::EnumSourceMapApplicator<testDomain::TestEnum, testDomain::TestEnum::Value> mapApplicator(
        mapValue, testEnum, babelwires::EnumToValueValueAdapter<testDomain::TestEnum>{testEnum});

    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Foo], testDomain::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Bar], testDomain::TestEnum::Value::Boo);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Erm], testDomain::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Oom], testDomain::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Boo], testDomain::TestEnum::Value::Erm);
}

TEST(MapHelperTest, enumSourceMapApplicator_allToSameFallback) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testDomain::TestEnum>();

    const testDomain::TestEnum& testEnum = typeSystem.getEntryByType<testDomain::TestEnum>();

    babelwires::MapValue mapValue = setUpTestEnumMapValue(typeSystem, mapValue, false);

    babelwires::EnumSourceMapApplicator<testDomain::TestEnum, testDomain::TestEnum::Value> mapApplicator(
        mapValue, testEnum, babelwires::EnumToValueValueAdapter<testDomain::TestEnum>{testEnum});

    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Foo], testDomain::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Bar], testDomain::TestEnum::Value::Boo);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Erm], testDomain::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Oom], testDomain::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Boo], testDomain::TestEnum::Value::Boo);
}

TEST(MapHelperTest, enumSourceMapApplicator_differentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testDomain::TestEnum>();

    const testDomain::TestEnum& testEnum = typeSystem.getEntryByType<testDomain::TestEnum>();

    babelwires::MapValue mapValue = setUpTestEnumTestTypeMapValue(typeSystem, mapValue);

    babelwires::EnumSourceMapApplicator<testDomain::TestEnum, std::string> mapApplicator(mapValue, testEnum,
                                                                                        &testValueAdapter);

    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Foo], "xxx");
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Bar], "yyy");
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Erm], "zzz");
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Oom], "zzz");
    EXPECT_EQ(mapApplicator[testDomain::TestEnum::Value::Boo], "zzz");
}
