#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SetMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::TextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::TextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::TextType::getThisIdentifier());

    const auto& textType = environment.m_typeSystem.getRegisteredType<babelwires::TextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *textType, *textType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *textType);

    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::TextValue(u8"Source"));
    oneToOne.setTargetValue(babelwires::TextValue(u8"Target"));

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *textType, *textType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::TextType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::TextValue(u8"Source2"));
    oneToOne2.setTargetValue(babelwires::TextValue(u8"Target2"));
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand testCopyConstructor("Set map",
                                                  babelwires::uniquePtrCast<babelwires::Value>(mapValue2.clone()));
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
    mapProject.setAllowedSourceTypeExps({{babelwires::TextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::TextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::TextType::getThisIdentifier());

    const auto& textType = environment.m_typeSystem.getRegisteredType<babelwires::TextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *textType, *textType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *textType);

    oneToOne.setSourceValue(babelwires::TextValue(u8"Source"));
    oneToOne.setTargetValue(babelwires::TextValue(u8"Target"));

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

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *textType, *textType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::TextType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::TextValue(u8"Source2"));
    oneToOne2.setTargetValue(babelwires::TextValue(u8"Target2"));
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand command("Set map", babelwires::uniquePtrCast<babelwires::Value>(mapValue2.clone()));

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
    mapProject.setAllowedSourceTypeExps({{babelwires::TextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::TextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::TextType::getThisIdentifier());

    const auto& textType = environment.m_typeSystem.getRegisteredType<babelwires::TextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *textType, *textType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *textType);

    oneToOne.setSourceValue(babelwires::TextValue(u8"Source"));
    oneToOne.setTargetValue(babelwires::TextValue(u8"Target"));

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *textType, *textType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::TextType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::TextType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::TextValue(u8"Source2"));
    oneToOne2.setTargetValue(babelwires::TextValue(u8"Target2"));
    mapValue2.emplaceBack(oneToOne2.clone());
    mapValue2.emplaceBack(allToOne.clone());
    mapValue2.emplaceBack(allToOne.clone());

    babelwires::SetMapCommand command("Set map", babelwires::uniquePtrCast<babelwires::Value>(mapValue2.clone()));

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
