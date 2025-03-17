#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(AddEntryToMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisType(),
                                              testUtils::TestType::getThisType());

    testUtils::TestValue nonDefaultValue("non-default");
    oneToOne.setSourceValue(nonDefaultValue);
    oneToOne.setTargetValue(nonDefaultValue);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, testUtils::TestType::getThisType());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand testCopyConstructor("Add entry", 1);
    babelwires::AddEntryToMapCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getSourceValue(), nonDefaultValue);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getTargetValue(), nonDefaultValue);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getSourceValue(), testUtils::TestValue("Default value"));
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getTargetValue(), testUtils::TestValue("Default value"));
    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getSourceValue(), nonDefaultValue);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getTargetValue(), nonDefaultValue);
}

TEST(AddEntryToMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisType(),
                                              testUtils::TestType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, testUtils::TestType::getThisType());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand command("Add entry", 2);

    EXPECT_FALSE(command.initialize(mapProject));
}
