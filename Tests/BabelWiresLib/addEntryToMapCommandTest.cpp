#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapProject.hpp>
#include <BabelWiresLib/Types/Map/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(AddEntryToMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    testUtils::TestValue newSourceValue;
    newSourceValue.m_value = "Source";
    testUtils::TestValue newTargetValue;
    newTargetValue.m_value = "Target";
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    babelwires::AddEntryToMapCommand command("Add entry", oneToOne.clone(), 1);

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne);
    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
}

TEST(AddEntryToMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisIdentifier());

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand command("Add entry", oneToOne.clone(), 2);

    EXPECT_FALSE(command.initialize(mapProject));
}
