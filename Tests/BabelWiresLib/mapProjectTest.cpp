#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::RegisteredTypeId testTypeId1 = "TestType1";
    const babelwires::RegisteredTypeId testTypeId2 = "TestType2";
} // namespace

TEST(MapProjectTest, mapProjectEntry) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    auto stringType = typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, *stringType, *stringType);

    babelwires::MapProjectEntry entry(oneToOne.clone());

    EXPECT_EQ(oneToOne, entry.getData());

    entry.validate(typeSystem, stringType, stringType, false);

    EXPECT_TRUE(entry.getValidity());

    entry.validate(typeSystem, stringType, stringType, true);
    EXPECT_FALSE(entry.getValidity());
    std::string reason = entry.getValidity().getReasonWhyFailed();
    EXPECT_TRUE(reason.find("fallback") != reason.npos);
}

TEST(MapProjectTest, allowedTypes) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedSourceTypeExps().m_typeExps,
                                                 {babelwires::StringType::getThisType()}));
    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedTargetTypeExps().m_typeExps,
                                                 {testDomain::TestEnum::getThisType()}));
}

TEST(MapProjectTest, getProjectContext) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    EXPECT_EQ(&environment.m_projectContext, &mapProject.getProjectContext());
}

TEST(MapProjectTest, types) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    mapProject.setCurrentSourceTypeExp(babelwires::StringType::getThisType());
    mapProject.setCurrentTargetTypeExp(testDomain::TestEnum::getThisType());

    EXPECT_EQ(mapProject.getCurrentSourceTypeExp(), babelwires::StringType::getThisType());
    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestEnum::getThisType());

    EXPECT_EQ(mapProject.getCurrentSourceType().get(),
              environment.m_typeSystem.getEntryByType<babelwires::StringType>().get());
    EXPECT_EQ(mapProject.getCurrentTargetType().get(),
              environment.m_typeSystem.getEntryByType<testDomain::TestEnum>().get());
}

TEST(MapProjectTest, badTypes) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeExp(testDomain::TestEnum::getThisType());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentTargetTypeExp(babelwires::StringType::getThisType());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeExp(babelwires::StringType::getThisType());
    mapProject.setCurrentTargetTypeExp(testDomain::TestEnum::getThisType());

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeExp(testTypeId1);
    mapProject.setCurrentTargetTypeExp(testTypeId2);

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());
}

TEST(MapProjectTest, setAndExtractMapValue) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();
    const auto& testSubEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *testSubEnumType);
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSubEnumType);

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentSourceTypeExp(), babelwires::StringType::getThisType());
    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubEnum::getThisType());
    EXPECT_EQ(mapProject.extractMapValue(), mapValue);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);
}

TEST(MapProjectTest, modifyMapValue) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();
    const auto& testSubEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *testSubEnumType);
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSubEnumType);

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *testSubEnumType);
    babelwires::StringValue newSourceValue;
    newSourceValue.set("Source");
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Erm");
    oneToOne2.setSourceValue(newSourceValue.clone());
    oneToOne2.setTargetValue(newTargetValue.clone());

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);

    mapProject.addMapEntry(oneToOne2.clone(), 1);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData(), allToOne);

    mapProject.removeMapEntry(0);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);

    babelwires::AllToOneFallbackMapEntryData allToOne2(environment.m_typeSystem, *testSubEnumType);
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Oom");
    allToOne2.setTargetValue(newTargetValue2.clone());

    mapProject.replaceMapEntry(allToOne2.clone(), 1);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne2);
}

TEST(MapProjectTest, typeChangeAndValidity) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{testDomain::TestEnum::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    const auto& testSubEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *testSubEnumType, *testSubEnumType);
    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Oom");
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue.clone());
    oneToOne.setTargetValue(newTargetValue.clone());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSubEnumType);
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Erm");
    allToOne.setTargetValue(newTargetValue2.clone());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(testDomain::TestSubEnum::getThisType());
    mapValue.setTargetTypeExp(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeExp(testDomain::TestSubSubEnum1::getThisType());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeExp(testDomain::TestEnum::getThisType());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeExp(testDomain::TestSubSubEnum1::getThisType());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeExp(testDomain::TestEnum::getThisType());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
}

TEST(MapProjectTest, modifyValidity) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();
    const auto& testEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestEnum>();
    const auto& testSubEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *testSubEnumType);
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSubEnumType);

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    // Wrong types
    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *testEnumType, *testSubEnumType);

    mapProject.addMapEntry(oneToOne2.clone(), 0);

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());

    // Only fallbacks allowed in the last position.
    babelwires::OneToOneMapEntryData oneToOne3(environment.m_typeSystem, *stringType, *testSubEnumType);

    mapProject.replaceMapEntry(oneToOne3.clone(), 2);

    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
}

TEST(MapProjectTest, badMap) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(testTypeId1);
    mapValue.setTargetTypeExp(testDomain::TestEnum::getThisType());

    mapProject.setMapValue(mapValue);

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(testTypeId2);
    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());
}
