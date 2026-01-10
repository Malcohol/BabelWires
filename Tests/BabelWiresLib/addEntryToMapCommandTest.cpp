#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(AddEntryToMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const babelwires::TypePtr& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType,
                                              *stringType);

    babelwires::StringValue nonDefaultValue("non-default");
    oneToOne.setSourceValue(nonDefaultValue.clone());
    oneToOne.setTargetValue(nonDefaultValue.clone());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand testCopyConstructor("Add entry", 1);
    babelwires::AddEntryToMapCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getSourceValue(), nonDefaultValue);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getTargetValue(), nonDefaultValue);
    EXPECT_EQ(*mapProject.getMapEntry(1).getData().getSourceValue(), babelwires::StringValue());
    EXPECT_EQ(*mapProject.getMapEntry(1).getData().getTargetValue(), babelwires::StringValue());
    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getSourceValue(), nonDefaultValue);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getTargetValue(), nonDefaultValue);
}

TEST(AddEntryToMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const babelwires::TypePtr& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();
    
    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType,
                                              *stringType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand command("Add entry", 2);

    EXPECT_FALSE(command.initialize(mapProject));
}
