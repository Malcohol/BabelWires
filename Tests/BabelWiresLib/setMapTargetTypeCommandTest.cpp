#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

TEST(SetMapTargetTypeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestEnum::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestSubSubEnum1::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisType(),
                                              testUtils::TestEnum::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestEnum::getThisType());

    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Erm");
    oneToOne.setTargetValue(newTargetValue);
    allToOne.setTargetValue(newTargetValue);

    mapValue.emplaceBack(oneToOne.clone());

    newTargetValue.set("Bar");
    oneToOne.setTargetValue(newTargetValue);
    mapValue.emplaceBack(oneToOne.clone());

    newTargetValue.set("Oom");
    oneToOne.setTargetValue(newTargetValue);
    mapValue.emplaceBack(oneToOne.clone());

    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testUtils::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapTargetTypeCommand testCopyConstructor("Set type", testUtils::TestSubSubEnum2::getThisType());
    babelwires::SetMapTargetTypeCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testUtils::TestSubSubEnum2::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testUtils::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapTargetTypeCommandTest, failWithUnallowedType) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                       testUtils::TestType::getThisType());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::SetMapTargetTypeCommand command("Set type", testUtils::TestEnum::getThisType());

    EXPECT_FALSE(command.initialize(mapProject));
}

