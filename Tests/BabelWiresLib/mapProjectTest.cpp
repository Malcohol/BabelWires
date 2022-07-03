#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(MapProjectTest, mapProjectEntry) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry(std::make_unique<testUtils::TestType>());

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::MapProjectEntry entry(oneToOne.clone());

    EXPECT_EQ(oneToOne, entry.getData());

    entry.validate(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier(),
                   false);

    EXPECT_TRUE(entry.getValidity());

    entry.validate(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier(),
                   true);

    EXPECT_FALSE(entry.getValidity());
    std::string reason = entry.getValidity().getReasonWhyFailed();
    EXPECT_TRUE(reason.find("fallback") != reason.npos);
}

TEST(MapProjectTest, allowedTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapProject.setAllowedTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(mapProject.getAllowedSourceTypeId(), testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(mapProject.getAllowedTargetTypeId(), testUtils::TestEnum::getThisIdentifier());
}

TEST(MapProjectTest, getProjectContext) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    EXPECT_EQ(&environment.m_projectContext, &mapProject.getProjectContext());
}

TEST(MapProjectTest, types) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapProject.setAllowedTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    mapProject.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapProject.setTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(mapProject.getSourceTypeId(), testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(mapProject.getTargetTypeId(), testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(mapProject.getSourceType(),
              environment.m_typeSystem.getEntryByIdentifier(testUtils::TestType::getThisIdentifier()));
    EXPECT_EQ(mapProject.getTargetType(),
              environment.m_typeSystem.getEntryByIdentifier(testUtils::TestEnum::getThisIdentifier()));
}

TEST(MapProjectTest, setAndExtractMapData) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapProject.setAllowedTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestSubEnum::getThisIdentifier());
    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    EXPECT_EQ(mapProject.getSourceTypeId(), testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(mapProject.getTargetTypeId(), testUtils::TestSubEnum::getThisIdentifier());
    EXPECT_EQ(mapProject.extractMapData(), mapData);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);
}

TEST(MapProjectTest, modifyMapData) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapProject.setAllowedTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestSubEnum::getThisIdentifier());
    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestSubEnum::getThisIdentifier());
    testUtils::TestValue newSourceValue;
    newSourceValue.m_value = "Source";
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

    babelwires::AllToOneFallbackMapEntryData allToOne2(environment.m_typeSystem,
                                                       testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Oom");
    allToOne2.setTargetValue(newTargetValue2.clone());

    mapProject.replaceMapEntry(allToOne2.clone(), 1);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne2);
}

TEST(MapProjectTest, changeTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());

    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeId(testUtils::TestEnum::getThisIdentifier());
    mapProject.setAllowedTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestSubEnum::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Oom");
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue.clone());
    oneToOne.setTargetValue(newTargetValue.clone());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Erm");
    allToOne.setTargetValue(newTargetValue2.clone());

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestSubEnum::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestSubEnum::getThisIdentifier());
    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setTargetTypeId(testUtils::TestSubSubEnum1::getThisIdentifier());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setTargetTypeId(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setSourceTypeId(testUtils::TestSubSubEnum1::getThisIdentifier());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setSourceTypeId(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
}
