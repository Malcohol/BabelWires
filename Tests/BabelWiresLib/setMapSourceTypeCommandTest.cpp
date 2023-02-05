#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/Commands/setMapSourceTypeCommand.hpp>

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

TEST(SetMapSourceTypeCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();
    testUtils::addTestEnumTypes(environment.m_typeSystem);

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeId({{testUtils::TestEnum::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeId({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapData mapData;
    mapData.setSourceTypeId(testUtils::TestSubSubEnum1::getThisIdentifier());
    mapData.setTargetTypeId(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestType::getThisIdentifier());

    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Erm");
    oneToOne.setSourceValue(newSourceValue.clone());
    mapData.emplaceBack(oneToOne.clone());

    newSourceValue.set("Bar");
    oneToOne.setSourceValue(newSourceValue.clone());
    mapData.emplaceBack(oneToOne.clone());

    newSourceValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue.clone());
    mapData.emplaceBack(oneToOne.clone());

    mapData.emplaceBack(allToOne.clone());

    mapProject.setMapData(mapData);

    EXPECT_EQ(mapProject.getSourceTypeId(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapSourceTypeCommand command("Set type", testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getSourceTypeId(), testUtils::TestSubSubEnum2::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getSourceTypeId(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapSourceTypeCommandTest, failWithUnallowedType) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment environment;
    environment.m_typeSystem.addEntry<testUtils::TestType>();
    environment.m_typeSystem.addEntry<testUtils::TestEnum>();

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

    babelwires::SetMapSourceTypeCommand command("Set type", testUtils::TestEnum::getThisIdentifier());

    EXPECT_FALSE(command.initialize(mapProject));
}
