#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::RegisteredTypeId testTypeId1 = "TestType1";
    const babelwires::RegisteredTypeId testTypeId2 = "TestType2";
} // namespace

TEST(MapProjectTest, mapProjectEntry) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                              babelwires::StringType::getThisType());

    babelwires::MapProjectEntry entry(oneToOne.clone());

    EXPECT_EQ(oneToOne, entry.getData());

    entry.validate(typeSystem, babelwires::StringType::getThisType(), babelwires::StringType::getThisType(),
                   false);

    EXPECT_TRUE(entry.getValidity());

    entry.validate(typeSystem, babelwires::StringType::getThisType(), babelwires::StringType::getThisType(),
                   true);

    EXPECT_FALSE(entry.getValidity());
    std::string reason = entry.getValidity().getReasonWhyFailed();
    EXPECT_TRUE(reason.find("fallback") != reason.npos);
}

TEST(MapProjectTest, allowedTypes) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedSourceTypeRefs().m_typeRefs, {babelwires::StringType::getThisType()}));
    EXPECT_TRUE(testUtils::unorderedAreEqualSets(mapProject.getAllowedTargetTypeRefs().m_typeRefs, {testDomain::TestEnum::getThisType()}));
}

TEST(MapProjectTest, getProjectContext) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);

    EXPECT_EQ(&environment.m_projectContext, &mapProject.getProjectContext());
}

TEST(MapProjectTest, types) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    mapProject.setCurrentSourceTypeRef(babelwires::StringType::getThisType());
    mapProject.setCurrentTargetTypeRef(testDomain::TestEnum::getThisType());

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), babelwires::StringType::getThisType());
    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testDomain::TestEnum::getThisType());

    EXPECT_EQ(mapProject.getCurrentSourceType(),
              &environment.m_typeSystem.getEntryByType<babelwires::StringType>());
    EXPECT_EQ(mapProject.getCurrentTargetType(),
              &environment.m_typeSystem.getEntryByType<testDomain::TestEnum>());
}

TEST(MapProjectTest, badTypes) {
    testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeRef(testDomain::TestEnum::getThisType());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentTargetTypeRef(babelwires::StringType::getThisType());

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());

    mapProject.setCurrentSourceTypeRef(babelwires::StringType::getThisType());
    mapProject.setCurrentTargetTypeRef(testDomain::TestEnum::getThisType());
    
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

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestSubEnum::getThisType());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testDomain::TestSubEnum::getThisType());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_EQ(mapProject.getCurrentSourceTypeRef(), babelwires::StringType::getThisType());
    EXPECT_EQ(mapProject.getCurrentTargetTypeRef(), testDomain::TestSubEnum::getThisType());
    EXPECT_EQ(mapProject.extractMapValue(), mapValue);
    EXPECT_EQ(mapProject.getNumMapEntries(), 2);
    EXPECT_EQ(mapProject.getMapEntry(0).getData(), oneToOne);
    EXPECT_EQ(mapProject.getMapEntry(1).getData(), allToOne);
}

TEST(MapProjectTest, modifyMapValue) {
    testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestSubEnum::getThisType());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testDomain::TestSubEnum::getThisType());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                               testDomain::TestSubEnum::getThisType());
    babelwires::StringValue newSourceValue;
    newSourceValue.set("Source");
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
                                                       testDomain::TestSubEnum::getThisType());
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

    mapProject.setAllowedSourceTypeRefs({{testDomain::TestEnum::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, testDomain::TestSubEnum::getThisType(),
                                              testDomain::TestSubEnum::getThisType());
    babelwires::EnumValue newSourceValue;
    newSourceValue.set("Oom");
    babelwires::EnumValue newTargetValue;
    newTargetValue.set("Oom");
    oneToOne.setSourceValue(newSourceValue);
    oneToOne.setTargetValue(newTargetValue);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testDomain::TestSubEnum::getThisType());
    babelwires::EnumValue newTargetValue2;
    newTargetValue2.set("Erm");
    allToOne.setTargetValue(newTargetValue2);

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testDomain::TestSubEnum::getThisType());
    mapValue.setTargetTypeRef(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeRef(testDomain::TestSubSubEnum1::getThisType());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentTargetTypeRef(testDomain::TestEnum::getThisType());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeRef(testDomain::TestSubSubEnum1::getThisType());

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());

    mapProject.setCurrentSourceTypeRef(testDomain::TestEnum::getThisType());

    EXPECT_TRUE(mapProject.getMapEntry(0).getValidity());
    EXPECT_TRUE(mapProject.getMapEntry(1).getValidity());
}

TEST(MapProjectTest, modifyValidity) {
     testUtils::TestEnvironment environment;
            
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestSubEnum::getThisType());
    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem,
                                                      testDomain::TestSubEnum::getThisType());

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(testDomain::TestSubEnum::getThisType());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());

    mapProject.setMapValue(mapValue);

    // Wrong types
    babelwires::OneToOneMapEntryData oneToOne2(environment.m_typeSystem, testDomain::TestEnum::getThisType(),
                                               testDomain::TestSubEnum::getThisType());

    mapProject.addMapEntry(oneToOne2.clone(), 0);

    EXPECT_FALSE(mapProject.getMapEntry(0).getValidity());

    // Only fallbacks allowed in the last position.
    babelwires::OneToOneMapEntryData oneToOne3(environment.m_typeSystem, babelwires::StringType::getThisType(),
                                               testDomain::TestSubEnum::getThisType());

    mapProject.replaceMapEntry(oneToOne3.clone(), 2);

    EXPECT_FALSE(mapProject.getMapEntry(2).getValidity());
}

TEST(MapProjectTest, badMap) {
     testUtils::TestEnvironment environment;
        
    babelwires::MapProject mapProject(environment.m_projectContext);

    mapProject.setAllowedSourceTypeRefs({{babelwires::StringType::getThisType()}});
    mapProject.setAllowedTargetTypeRefs({{testDomain::TestEnum::getThisType()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testDomain::TestEnum::getThisType());

    mapProject.setMapValue(mapValue);

    EXPECT_FALSE(mapProject.getSourceTypeValidity());
    EXPECT_TRUE(mapProject.getTargetTypeValidity());

    mapValue.setSourceTypeRef(babelwires::StringType::getThisType());
    mapValue.setTargetTypeRef(testTypeId2);
    mapProject.setMapValue(mapValue);

    EXPECT_TRUE(mapProject.getSourceTypeValidity());
    EXPECT_FALSE(mapProject.getTargetTypeValidity());
}
