#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

TEST(SetMapTargetTypeCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    testUtils::addTestEnumTypes(environment.m_typeSystem);

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestEnum::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestSubSubEnum1::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestEnum::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestEnum::getThisIdentifier());

    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Erm");
    oneToOne.setTargetValue(newTargetValue.clone());
    allToOne.setTargetValue(newTargetValue.clone());

    mapData.emplaceBack(oneToOne.clone());

    newTargetValue.set("Bar");
    oneToOne.setTargetValue(newTargetValue.clone());
    mapData.emplaceBack(oneToOne.clone());

    newTargetValue.set("Oom");
    oneToOne.setTargetValue(newTargetValue.clone());
    mapData.emplaceBack(oneToOne.clone());

    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    EXPECT_EQ(mapProject.getTargetTypeId(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapTargetTypeCommand command("Set type", testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getTargetTypeId(), testUtils::TestSubSubEnum2::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getTargetTypeId(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapTargetTypeCommandTest, failWithUnallowedType) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    environment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());
    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    babelwires::SetMapTargetTypeCommand command("Set type", testUtils::TestEnum::getThisIdentifier());

    EXPECT_FALSE(command.initialize(mapProject));
}

