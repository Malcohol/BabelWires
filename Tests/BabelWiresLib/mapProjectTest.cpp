#include <gtest/gtest.h>

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

#include <Tests/TestUtils/equalSets.hpp>

namespace {
    const babelwires::PrimitiveTypeId testTypeId1 = "TestType1";
    const babelwires::PrimitiveTypeId testTypeId2 = "TestType2";
} // namespace

TEST(MapProjectTest, mapProjectEntry) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestType::getThisIdentifier());

    babelwires::MapProjectEntry entry(oneToOne.clone());

    EXPECT_EQ(oneToOne, entry.getData());

    entry.validate(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier(),
                   false);

    EXPECT_TRUE(entry.getValidity());

    entry.validate(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier(),
                   true);

    EXPECT_FALSE(entry.getValidity());
    std::string reason = entry.getValidity().getReasonWhyFailed();
    EXPECT_TRUE(reason.find("fallback") != reason.npos);
}

TEST(MapProjectTest, allowedTypes) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedSourceTypeRefs().m_typeRefs, {testUtils::TestType::getThisIdentifier()}));
    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedTargetTypeRefs().m_typeRefs, {testUtils::TestEnum::getThisIdentifier()}));
}

TEST(MapProjectTest, getProjectContext) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    EXPECT_EQ(&environment.m_projectContext, &mapProject.getProjectContext());
}

TEST(MapProjectTest, types) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    mapProject.setCurrentSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setCurrentTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testUtils::TestEnum::getThisIdentifier());

    EXPECT_EQ(mapProject.getCurrentSourceType(),
              &environment.m_typeSystem.getEntryByType<testUtils::TestType>());
    EXPECT_EQ(mapProject.getCurrentTargetType(),
              &environment.m_typeSystem.getEntryByType<testUtils::TestEnum>());
}

TEST(MapProjectTest, badTypes) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentTargetTypeRef(testUtils::TestType::getThisIdentifier());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setCurrentTargetTypeRef(testUtils::TestEnum::getThisIdentifier());
    
    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeRef(testTypeId1);
    mapProject.setCurrentTargetTypeRef(testTypeId2);

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());
}

TEST(MapProjectTest, setAndExtractMapValue) {
    testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestSubEnum::getThisIdentifier());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), testUtils::TestType::getThisIdentifier());
    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testUtils::TestSubEnum::getThisIdentifier());
    EXPECT_EQ(mapProject.extractMapValue(), mapValue);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);
}

TEST(MapProjectTest, modifyMapValue) {
    testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestSubEnum::getThisIdentifier());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestSubEnum::getThisIdentifier());
    testUtils::TestValue newSourceValue;
    newSourceValue.m_value = "Source";
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Erm");
    oneToOne2.setSourceValue(newSourceValue);
    oneToOne2.setTargetValue(newTargetValue);

    EXPECT_EQ(mapProject.getNumMapEntries(), 2);

    mapProject.addMapEntry(oneToOne2.clone(), 1);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(2).getData(), allToOne);

    mapProject.removeMapEntry(0);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);

    babelwires::AllToOneFallbackMapEntryData allToOne2(environment.m_typeSystem,
                                                       testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Oom");
    allToOne2.setTargetValue(newTargetValue2);

    mapProject.replaceMapEntry(allToOne2.clone(), 1);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne2);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne2);
}

TEST(MapProjectTest, typeChangeAndValidity) {
    testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestEnum::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestSubEnum::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Oom");
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Erm");
    allToOne.setTargetValue(newTargetValue2);

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestSubEnum::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestSubEnum::getThisIdentifier());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeRef(testUtils::TestSubSubEnum1::getThisIdentifier());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeRef(testUtils::TestSubSubEnum1::getThisIdentifier());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeRef(testUtils::TestEnum::getThisIdentifier());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
}

TEST(MapProjectTest, modifyValidity) {
     testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestSubEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testUtils::TestSubEnum::getThisIdentifier());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testUtils::TestSubEnum::getThisIdentifier());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    // Wrong types
    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                               testUtils::TestSubEnum::getThisIdentifier());

    mapProject.addMapEntry(oneToOne2.clone(), 0);

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());

    // Only fallbacks allowed in the last position.
    babelwires::OneToOneMapEntryData oneToOne3(environment.m_typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestSubEnum::getThisIdentifier());

    mapProject.replaceMapEntry(oneToOne3.clone(), 2);

    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
}

TEST(MapProjectTest, badMap) {
     testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setSpecifiedSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapProject.setSpecifiedTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testUtils::TestEnum::getThisIdentifier());

    mapProject.setMapValue(mapValue);

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapValue.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapValue.setTargetTypeRef(testTypeId2);
    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());
}
