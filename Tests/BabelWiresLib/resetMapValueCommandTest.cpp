#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/Commands/resetMapValueCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testSumType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ResetMapValueCommandTest, executeAndUndoSource) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{testDomain::TestSumType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestSumType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(testDomain::TestSumType::getThisIdentifier());
    mapValue.setTargetTypeExp(testDomain::TestSumType::getThisIdentifier());

    const auto& testSumType = environment.m_typeSystem.getRegisteredType<testDomain::TestSumType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *testSumType, *testSumType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    mapProject.setMapValue(mapValue);

    babelwires::MapProjectDataLocation loc(1, babelwires::MapProjectDataLocation::Side::source, babelwires::Path());
    babelwires::ResetMapValueCommand testCopyConstructor("Reset", loc, babelwires::DefaultIntType::getThisIdentifier());
    babelwires::ResetMapValueCommand command = testCopyConstructor;

    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().getSourceValue()->tryAs<babelwires::RationalValue>(), nullptr);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All2Sm);

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().getSourceValue()->tryAs<babelwires::IntValue>(), nullptr);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All2Sm);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().getSourceValue()->tryAs<babelwires::RationalValue>(), nullptr);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All2Sm);
}

TEST(ResetMapValueCommandTest, executeAndUndoTarget) {
    testUtils::TestEnvironment environment;

    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{testDomain::TestSumType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestSumType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(testDomain::TestSumType::getThisIdentifier());
    mapValue.setTargetTypeExp(testDomain::TestSumType::getThisIdentifier());

    const auto& testSumType = environment.m_typeSystem.getRegisteredType<testDomain::TestSumType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *testSumType, *testSumType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSumType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    // Reset the target of the fallback.
    babelwires::MapProjectDataLocation loc(2, babelwires::MapProjectDataLocation::Side::target, babelwires::Path());
    babelwires::ResetMapValueCommand testCopyConstructor("Reset", loc, babelwires::DefaultIntType::getThisIdentifier());
    babelwires::ResetMapValueCommand command = testCopyConstructor;

    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_NE(mapProject.getMapEntry(1).getData().getTargetValue()->tryAs<babelwires::RationalValue>(), nullptr);

    EXPECT_TRUE(command.initialize(mapProject));
    command.execute(mapProject);

    EXPECT_EQ(mapProject.getNumMapEntries(), 3);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_NE(mapProject.getMapEntry(2).getData().getTargetValue()->tryAs<babelwires::IntValue>(), nullptr);

    command.undo(mapProject);
    EXPECT_EQ(mapProject.getMapEntry(0).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(1).getData().getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(mapProject.getMapEntry(2).getData().getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_NE(mapProject.getMapEntry(2).getData().getTargetValue()->tryAs<babelwires::RationalValue>(), nullptr);
}

TEST(ResetMapValueCommandTest, failBeyondEnd) {
    testUtils::TestEnvironment environment;
    
    babelwires::MapProject mapProject(environment.m_projectContext);
    mapProject.setAllowedSourceTypeExps({{testDomain::TestSumType::getThisIdentifier()}});
    mapProject.setAllowedTargetTypeExps({{testDomain::TestSumType::getThisIdentifier()}});

    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(testDomain::TestSumType::getThisIdentifier());
    mapValue.setTargetTypeExp(testDomain::TestSumType::getThisIdentifier());

    const auto& testSumType = environment.m_typeSystem.getRegisteredType<testDomain::TestSumType>();

    babelwires::OneToOneMapEntryData oneToOne(environment.m_typeSystem, *testSumType, *testSumType);

    babelwires::AllToOneFallbackMapEntryData allToOne(environment.m_typeSystem, *testSumType);

    mapValue.emplaceBack(oneToOne.clone());
    mapValue.emplaceBack(allToOne.clone());
    mapProject.setMapValue(mapValue);

    babelwires::MapProjectDataLocation loc(2, babelwires::MapProjectDataLocation::Side::target, babelwires::Path());
    babelwires::ResetMapValueCommand command("Reset", loc, babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_FALSE(command.initialize(mapProject));    
}
