#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(SetMapTargetTypeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestEnum::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(testDomain::TestSubSubEnum1::getThisIdentifier());

    const auto& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();
    const auto& testEnumType = environment.m_typeSystem.getRegisteredType<testDomain::TestEnum>();

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

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapTargetTypeCommand testCopyConstructor("Set type", testDomain::TestSubSubEnum2::getThisIdentifier());
    babelwires::SetMapTargetTypeCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum2::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getCurrentTargetTypeExp(), testDomain::TestSubSubEnum1::getThisIdentifier());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapTargetTypeCommandTest, failWithUnallowedType) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{babelwires::StringType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{babelwires::StringType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(babelwires::StringType::getThisIdentifier());
    mapValue.setTargetTypeExp(babelwires::StringType::getThisIdentifier());

    const auto& stringType = environment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *stringType);
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::SetMapTargetTypeCommand command("Set type", testDomain::TestEnum::getThisIdentifier());

    EXPECT_FALSE(command.initialize(mapProject));
}

