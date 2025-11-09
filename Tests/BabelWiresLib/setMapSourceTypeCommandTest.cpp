#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/setMapSourceTypeCommand.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

TEST(SetMapSourceTypeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{testDomain::TestEnum::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testDomain::TestSubSubEnum1::getThisType());
    mapValue.setTargetTypeRef(babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testDomain::TestEnum::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, babelwires::StringType::getThisType());

    oneToOne.setSourceValue(babelwires::EnumValue("Erm"));
    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::EnumValue("Bar"));
    mapValue.emplaceBack(oneToOne.clone());

    oneToOne.setSourceValue(babelwires::EnumValue("Oom"));
    mapValue.emplaceBack(oneToOne.clone());

    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testDomain::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    babelwires::SetMapSourceTypeCommand testCopyConstructor("Set type", testDomain::TestSubSubEnum2::getThisType());
    babelwires::SetMapSourceTypeCommand command = testCopyConstructor;

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testDomain::TestSubSubEnum2::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(1).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());

    command.undo(mapProject);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testDomain::TestSubSubEnum1::getThisType());
    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(3).getValidity());
}

TEST(SetMapSourceTypeCommandTest, failWithUnallowedType) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{babelwires::StringType::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, babelwires::StringType::getThisType());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::SetMapSourceTypeCommand command("Set type", testDomain::TestEnum::getThisType());

    EXPECT_FALSE(command.initialize(mapProject));
}
