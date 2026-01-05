#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(SetMapTargetTypeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(testDomain::TestSubSubEnum1::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();
    const auto& testEnumType = environment.m_typeSystem.getEntryByType<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *stringType, *testEnumType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testEnumType);

    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Erm");
    oneToOne.setTargetValue(newTargetValue.clone());
    allToOne.setTargetValue(newTargetValue.clone());

    mapValue.emplaceBack(oneToOne.clone());

    newTargetValue.set("Bar");
    oneToOne.setTargetValue(newTargetValue.clone());
    mapValue.emplaceBack(oneToOne.clone());

    newTargetValue.set("Oom");
    oneToOne.setTargetValue(newTargetValue.clone());
    mapValue.emplaceBack(oneToOne.clone());

    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapTargetTypeCommand testCopyConstructor("Set type", testDomain::TestSubSubEnum2::getThisType());
    babelwires::SetMapTargetTypeCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum2::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapTargetTypeCommandTest, failWithUnallowedType) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisType());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisType());

    const auto& stringType = environment.m_typeSystem.getEntryByType<babelwires::StringType>();

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::SetMapTargetTypeCommand command("Set type", testDomain::TestEnum::getThisType());

    EXPECT_FALSE(command.initialize(mapProject));
}

