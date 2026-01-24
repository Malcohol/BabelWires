#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SetMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const auto& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::StringValue("Source"));
    oneToOne.setTargetValue(babelwires::StringValue("Target"));

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *stringType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::StringType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::StringValue("Source2"));
    oneToOne2.setTargetValue(babelwires::StringValue("Target2"));
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
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const auto& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    oneToOne.setSourceValue(babelwires::StringValue("Source"));
    oneToOne.setTargetValue(babelwires::StringValue("Target"));

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

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *stringType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::StringType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::StringValue("Source2"));
    oneToOne2.setTargetValue(babelwires::StringValue("Target2"));
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
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const auto& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    oneToOne.setSourceValue(babelwires::StringValue("Source"));
    oneToOne.setTargetValue(babelwires::StringValue("Target"));

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, *stringType, *stringType);

    babelwires::MapValue mapValue2;
    mapValue2.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue2.setTargetTypeExp(babelwires::StringType::getThisIdentifier());
    oneToOne2.setSourceValue(babelwires::StringValue("Source2"));
    oneToOne2.setTargetValue(babelwires::StringValue("Target2"));
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
