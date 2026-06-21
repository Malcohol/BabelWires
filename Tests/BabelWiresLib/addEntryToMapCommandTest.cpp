#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(AddEntryToMapCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const babelwires::TypePtr& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType,
                                              *defaultTextType);

    babelwires::TextValue nonDefaultValue(u8"non-default");
    oneToOne.setSourceValue(nonDefaultValue.clone());
    oneToOne.setTargetValue(nonDefaultValue.clone());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

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
    EXPECT_EQ(*mapProject.getMapEntry(1).getData().getSourceValue(), babelwires::TextValue());
    EXPECT_EQ(*mapProject.getMapEntry(1).getData().getTargetValue(), babelwires::TextValue());
    command.undo(mapProject);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getSourceValue(), nonDefaultValue);
    EXPECT_EQ(*mapProject.getMapEntry(0).getData().getTargetValue(), nonDefaultValue);
}

TEST(AddEntryToMapCommandTest, failAtEnd) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const babelwires::TypePtr& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();
    
    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType,
                                              *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::AddEntryToMapCommand command("Add entry", 2);

    EXPECT_FALSE(command.initialize(mapProject));
}
