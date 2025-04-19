#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/removeEntryFromMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(RemoveEntryFromMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       babelwires::StringType::getThisType());

    mapValue.emplaceBack(oneToOne.clone());

    babelwires::StringValue newSourceValue;
    newSourceValue.set("Source");
    babelwires::StringValue newTargetValue;
    newTargetValue.set("Target");
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);

    babelwires::RemoveEntryFromMapCommand testCopyConstructor("Remove", 1);
    babelwires::RemoveEntryFromMapCommand command = testCopyConstructor;
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
}

TEST(RemoveEntryFromMapCommandTest, removeInvalid) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       babelwires::StringType::getThisType());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
}

TEST(RemoveEntryFromMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       babelwires::StringType::getThisType());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    EXPECT_FALSE(command.initialize(mapProject));    
}
