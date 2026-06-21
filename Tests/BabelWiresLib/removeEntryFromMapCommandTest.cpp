#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/removeEntryFromMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(RemoveEntryFromMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const auto& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::TextValue(u8"Source"));
    oneToOne.setTargetValue(babelwires::TextValue(u8"Target"));

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
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const auto& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

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
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const auto& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::RemoveEntryFromMapCommand command("Remove", 1);
    EXPECT_FALSE(command.initialize(mapProject));    
}
