#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SetMapCommandTest, executeAndUndo) {
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
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue2.setTargetTypeRef(babelwires::StringType::getThisType());
    newSourceValue.set("Source2");
    newTargetValue.set("Target2");
    oneToOne2.setSourceValue(newSourceValue);
    oneToOne2.setTargetValue(newTargetValue);
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand testCopyConstructor("Set map", mapValue2.clone());
    babelwires::SetMapCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
}

TEST(SetMapCommandTest, invalidOldMap) {
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

    babelwires::StringValue newSourceValue;
    newSourceValue.set("Source");
    babelwires::StringValue newTargetValue;
    newTargetValue.set("Target");
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue2.setTargetTypeRef(babelwires::StringType::getThisType());
    newSourceValue.set("Source2");
    newTargetValue.set("Target2");
    oneToOne2.setSourceValue(newSourceValue);
    oneToOne2.setTargetValue(newTargetValue);
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand command("Set map", mapValue2.clone());

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
}

TEST(SetMapCommandTest, invalidNewMap) {
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

    babelwires::StringValue newSourceValue;
    newSourceValue.set("Source");
    babelwires::StringValue newTargetValue;
    newTargetValue.set("Target");
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue2.setTargetTypeRef(babelwires::StringType::getThisType());
    newSourceValue.set("Source2");
    newTargetValue.set("Target2");
    oneToOne2.setSourceValue(newSourceValue);
    oneToOne2.setTargetValue(newTargetValue);
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand command("Set map", mapValue2.clone());

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
}

