#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/Helpers/enumSourceMapApplicator.hpp>
#include <BabelWiresLib/Maps/Helpers/enumValueAdapters.hpp>
#include <BabelWiresLib/Maps/Helpers/unorderedMapApplicator.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

namespace {
    std::string testValueAdapter(const babelwires::Value& value) {
        return value.as<testUtils::TestValue>()->m_value;
    }

    babelwires::MapData setUpTestMapData(const babelwires::TypeSystem& typeSystem,
                                         babelwires::LongId sourceTypeId,
                                         babelwires::LongId targetTypeId, const babelwires::Value& sourceValue1,
                                         const babelwires::Value& sourceValue2, const babelwires::Value& targetValue1,
                                         const babelwires::Value& targetValue2, const babelwires::Value& targetValue3,
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

        babelwires::MapData mapData;
        mapData.setSourceTypeRef(sourceTypeId);
        mapData.setTargetTypeRef(targetTypeId);
        mapData.emplaceBack(std::move(oneToOne1));
        mapData.emplaceBack(std::move(oneToOne2));
        mapData.emplaceBack(std::move(fallback));
        return mapData;
    }

    babelwires::MapData setUpTestTypeMapData(const babelwires::TypeSystem& typeSystem, babelwires::MapData& mapData,
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

        return setUpTestMapData(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier(),
                         sourceValue1, sourceValue2, targetValue1, targetValue2, targetValue3,
                         allToOneFallback);
    }

    babelwires::MapData setUpTestEnumMapData(const babelwires::TypeSystem& typeSystem, babelwires::MapData& mapData,
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

        return setUpTestMapData(typeSystem, testUtils::TestEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier(),
                         sourceValue1, sourceValue2, targetValue1, targetValue2, targetValue3,
                         allToOneFallback);
    }

    babelwires::MapData setUpTestTypeTestEnumMapData(const babelwires::TypeSystem& typeSystem, babelwires::MapData& mapData) {
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

        return setUpTestMapData(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier(),
                         sourceValue1, sourceValue2, targetValue1, targetValue2, targetValue3, true);
    }

    babelwires::MapData setUpTestEnumTestTypeMapData(const babelwires::TypeSystem& typeSystem, babelwires::MapData& mapData) {
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

        return setUpTestMapData(typeSystem, testUtils::TestEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier(),
                         sourceValue1, sourceValue2, targetValue1, targetValue2, targetValue3,
                         true);
    }
} // namespace

TEST(MapHelperTest, unorderedMapApplicator_allToOneFallback) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapData mapData = setUpTestTypeMapData(typeSystem, mapData, true);

    babelwires::UnorderedMapApplicator<std::string, std::string> mapApplicator(mapData, &testValueAdapter,
                                                                               &testValueAdapter);

    EXPECT_EQ(mapApplicator["aaa"], "xxx");
    EXPECT_EQ(mapApplicator["bbb"], "yyy");
    EXPECT_EQ(mapApplicator["ccc"], "zzz");
    EXPECT_EQ(mapApplicator["ddd"], "zzz");
}

TEST(MapHelperTest, unorderedMapApplicator_allToSameFallback) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapData mapData = setUpTestTypeMapData(typeSystem, mapData, false);

    babelwires::UnorderedMapApplicator<std::string, std::string> mapApplicator(mapData, &testValueAdapter,
                                                                               &testValueAdapter);

    EXPECT_EQ(mapApplicator["aaa"], "xxx");
    EXPECT_EQ(mapApplicator["bbb"], "yyy");
    EXPECT_EQ(mapApplicator["ccc"], "ccc");
    EXPECT_EQ(mapApplicator["ddd"], "ddd");
}

TEST(MapHelperTest, unorderedMapApplicator_differentTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    const testUtils::TestEnum& testEnum = typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::MapData mapData = setUpTestTypeTestEnumMapData(typeSystem, mapData);

    // Also test the EnumToIndexValueAdapter
    babelwires::UnorderedMapApplicator<std::string, unsigned int> mapApplicator(
        mapData, &testValueAdapter, babelwires::EnumToIndexValueAdapter{testEnum});

    EXPECT_EQ(mapApplicator["aaa"], 3);
    EXPECT_EQ(mapApplicator["bbb"], 4);
    EXPECT_EQ(mapApplicator["ccc"], 2);
    EXPECT_EQ(mapApplicator["ddd"], 2);
}

TEST(MapHelperTest, enumSourceMapApplicator_allToOneFallback) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestEnum>();

    const testUtils::TestEnum& testEnum = typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::MapData mapData = setUpTestEnumMapData(typeSystem, mapData, true);

    babelwires::EnumSourceMapApplicator<testUtils::TestEnum, testUtils::TestEnum::Value> mapApplicator(
        mapData, testEnum, babelwires::EnumToValueValueAdapter<testUtils::TestEnum>{testEnum});

    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Foo], testUtils::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Bar], testUtils::TestEnum::Value::Boo);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Erm], testUtils::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Oom], testUtils::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Boo], testUtils::TestEnum::Value::Erm);
}

TEST(MapHelperTest, enumSourceMapApplicator_allToSameFallback) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestEnum>();

    const testUtils::TestEnum& testEnum = typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::MapData mapData = setUpTestEnumMapData(typeSystem, mapData, false);

    babelwires::EnumSourceMapApplicator<testUtils::TestEnum, testUtils::TestEnum::Value> mapApplicator(
        mapData, testEnum, babelwires::EnumToValueValueAdapter<testUtils::TestEnum>{testEnum});

    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Foo], testUtils::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Bar], testUtils::TestEnum::Value::Boo);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Erm], testUtils::TestEnum::Value::Erm);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Oom], testUtils::TestEnum::Value::Oom);
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Boo], testUtils::TestEnum::Value::Boo);
}

TEST(MapHelperTest, enumSourceMapApplicator_differentTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestEnum>();

    const testUtils::TestEnum& testEnum = typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::MapData mapData = setUpTestEnumTestTypeMapData(typeSystem, mapData);

    babelwires::EnumSourceMapApplicator<testUtils::TestEnum, std::string> mapApplicator(
        mapData, testEnum, &testValueAdapter);

    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Foo], "xxx");
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Bar], "yyy");
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Erm], "zzz");
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Oom], "zzz");
    EXPECT_EQ(mapApplicator[testUtils::TestEnum::Value::Boo], "zzz");
}
