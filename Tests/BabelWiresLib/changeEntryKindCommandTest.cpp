#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/changeEntryKindCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ChangeEntryKindCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const babelwires::TypePtr& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::ChangeEntryKindCommand testCopyConstructor("Set kind", babelwires::MapEntryData::Kind::One21, 1);
    babelwires::ChangeEntryKindCommand command = testCopyConstructor;

    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().tryAs<babelwires::AllToOneFallbackMapEntryData>(), nullptr);

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().tryAs<babelwires::OneToOneMapEntryData>(), nullptr);
    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().tryAs<babelwires::AllToOneFallbackMapEntryData>(), nullptr);
}

TEST(ChangeEntryKindCommandTest, failFallbackNotAtEnd) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const babelwires::TypePtr& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::ChangeEntryKindCommand command("Set kind", babelwires::MapEntryData::Kind::All21, 1);
    EXPECT_FALSE(command.initialize(mapProject));
}

TEST(ChangeEntryKindCommandTest, failNotFallbackAtEnd) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::DefaultTextType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::DefaultTextType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::DefaultTextType::getThisIdentifier());

    const babelwires::TypePtr& defaultTextType = environment.m_typeSystem.getRegisteredType<babelwires::DefaultTextType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *defaultTextType, *defaultTextType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *defaultTextType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::ChangeEntryKindCommand command("Set kind", babelwires::MapEntryData::Kind::One21, 2);
    EXPECT_FALSE(command.initialize(mapProject));
}
