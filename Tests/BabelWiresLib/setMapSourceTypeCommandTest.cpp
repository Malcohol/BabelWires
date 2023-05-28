#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapSourceTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(SetMapSourceTypeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestEnum::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestSubSubEnum1::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestType::getThisIdentifier());

    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Erm");
    oneToOne.setSourceValue(newSourceValue);
    mapValue.emplaceBack(oneToOne.clone());

    newSourceValue.set("Bar");
    oneToOne.setSourceValue(newSourceValue);
    mapValue.emplaceBack(oneToOne.clone());

    newSourceValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue);
    mapValue.emplaceBack(oneToOne.clone());

    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapSourceTypeCommand command("Set type", testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testUtils::TestSubSubEnum2::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testUtils::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapSourceTypeCommandTest, failWithUnallowedType) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testUtils::TestType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeRefs({{testUtils::TestType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestType::getThisIdentifier());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::SetMapSourceTypeCommand command("Set type", testUtils::TestEnum::getThisIdentifier());

    EXPECT_FALSE(command.initialize(mapProject));
}
