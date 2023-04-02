#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/Commands/removeEntryFromMapCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(RemoveEntryFromMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());

    testUtils::TestValue newSourceValue;
    newSourceValue.m_value = "Source";
    testUtils::TestValue newTargetValue;
    newTargetValue.m_value = "Target";
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
}

TEST(RemoveEntryFromMapCommandTest, removeInvalid) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
}

TEST(RemoveEntryFromMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapData.emplaceBack(oneToOne.clone());
    mapData.emplaceBack(allToOne.clone());
    mapProject.setMapData(mapData);

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    EXPECT_FALSE(command.initialize(mapProject));    
}
