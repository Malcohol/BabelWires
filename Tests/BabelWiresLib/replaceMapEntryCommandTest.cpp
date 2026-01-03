#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/replaceMapEntryCommand.hpp>

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

TEST(ReplaceMapEntryCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::StringValue("Source"));
    oneToOne.setTargetValue(babelwires::StringValue("Target"));

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *stringType);
    oneToOne2.setSourceValue(babelwires::StringValue("Source2"));
    oneToOne2.setTargetValue(babelwires::StringValue("Target2"));

    babelwires::ReplaceMapEntryCommand testCopyConstructor("Replace", oneToOne2.clone(), 1);
    babelwires::ReplaceMapEntryCommand command = testCopyConstructor;
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
}

TEST(ReplaceMapEntryCommandTest, failBeyondEnd) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::ReplaceMapEntryCommand command("Replace", oneToOne.clone(), 2);
    EXPECT_FALSE(command.initialize(mapProject));    
}

TEST(ReplaceMapEntryCommandTest, replaceInvalid) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *stringType);
    
    oneToOne2.setSourceValue(babelwires::StringValue("Source"));
    oneToOne2.setTargetValue(babelwires::StringValue("Target"));

    babelwires::ReplaceMapEntryCommand command("Replace", oneToOne2.clone(), 1);
    
    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
}

TEST(ReplaceMapEntryCommandTest, failInvalid) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::ReplaceMapEntryCommand command("Replace", allToOne.clone(), 0);
    EXPECT_FALSE(command.initialize(mapProject));    
}
