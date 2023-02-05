#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/Commands/replaceMapEntryCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(ReplaceMapEntryCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());

    testUtils::TestValue newSourceValue;
    newSourceValue.m_value = "Source";
    testUtils::TestValue newTargetValue;
    newTargetValue.m_value = "Target";
    oneToOne.setSourceValue(newSourceValue.clone());
    oneToOne.setTargetValue(newTargetValue.clone());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());
    newSourceValue.m_value = "Source2";
    newTargetValue.m_value = "Target2";
    oneToOne2.setSourceValue(newSourceValue.clone());
    oneToOne2.setTargetValue(newTargetValue.clone());

    babelwires::ReplaceMapEntryCommand command("Replace", oneToOne2.clone(), 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
}

TEST(ReplaceMapEntryCommandTest, failBeyondEnd) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    babelwires::ReplaceMapEntryCommand command("Replace", oneToOne.clone(), 2);
    EXPECT_FALSE(command.initialize(mapProject));    
}

TEST(ReplaceMapEntryCommandTest, replaceInvalid) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());
    testUtils::TestValue newSourceValue;
    testUtils::TestValue newTargetValue;
    newSourceValue.m_value = "Source2";
    newTargetValue.m_value = "Target2";
    oneToOne2.setSourceValue(newSourceValue.clone());
    oneToOne2.setTargetValue(newTargetValue.clone());
    
    babelwires::ReplaceMapEntryCommand command("Replace", oneToOne2.clone(), 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
}

TEST(ReplaceMapEntryCommandTest, failInvalid) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    babelwires::ReplaceMapEntryCommand command("Replace", allToOne.clone(), 0);
    EXPECT_FALSE(command.initialize(mapProject));    
}
