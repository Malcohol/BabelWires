#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::LongId testTypeId1 = "TestType1";
    const babelwires::LongId testTypeId2 = "TestType2";
} // namespace

TEST(MapDataTest, types) {
    babelwires::MapData mapData;

    EXPECT_EQ(mapData.getSourceTypeRef(), babelwires::TypeRef());
    EXPECT_EQ(mapData.getTargetTypeRef(), babelwires::TypeRef());

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);
    EXPECT_EQ(mapData.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapData.getTargetTypeRef(), testTypeId2);
}

TEST(MapDataTest, entries) {
    babelwires::MapData mapData;

    EXPECT_EQ(mapData.getNumMapEntries(), 0);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    EXPECT_EQ(mapData.getNumMapEntries(), 1);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    EXPECT_EQ(mapData.getNumMapEntries(), 2);
}

TEST(MapDataTest, copyConstruction) {
    babelwires::MapData mapData;

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    babelwires::MapData mapData2 = mapData;
    EXPECT_EQ(mapData2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapData2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapData2.getNumMapEntries(), 1);
    // Ensure mapEntries are not shared.
    EXPECT_NE(&mapData.getMapEntry(0), &mapData2.getMapEntry(0));
}

TEST(MapDataTest, moveConstruction) {
    babelwires::MapData mapData;

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    auto mapEntry = &mapData.getMapEntry(0);

    babelwires::MapData mapData2 = std::move(mapData);
    EXPECT_EQ(mapData2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapData2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapData2.getNumMapEntries(), 1);
    // Ensure mapEntries are moved.
    EXPECT_EQ(&mapData2.getMapEntry(0), mapEntry);
}

TEST(MapDataTest, copyAssignment) {
    babelwires::MapData mapData;

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    babelwires::MapData mapData2;
    mapData2 = mapData;
    EXPECT_EQ(mapData2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapData2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapData2.getNumMapEntries(), 1);
    // Ensure mapEntries are not shared.
    EXPECT_NE(&mapData.getMapEntry(0), &mapData2.getMapEntry(0));
}

TEST(MapDataTest, moveAssignment) {
    babelwires::MapData mapData;

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    auto mapEntry = &mapData.getMapEntry(0);

    babelwires::MapData mapData2;
    mapData2 = std::move(mapData);
    EXPECT_EQ(mapData2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapData2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapData2.getNumMapEntries(), 1);
    // Ensure mapEntries are moved.
    EXPECT_EQ(&mapData2.getMapEntry(0), mapEntry);
}

TEST(MapDataTest, equality) {
    babelwires::MapData mapData;
    babelwires::MapData mapData2;

    EXPECT_TRUE(mapData == mapData2);
    EXPECT_FALSE(mapData != mapData2);

    mapData.setSourceTypeRef(testTypeId1);

    EXPECT_FALSE(mapData == mapData2);
    EXPECT_TRUE(mapData != mapData2);

    mapData2.setSourceTypeRef(testTypeId1);

    EXPECT_TRUE(mapData == mapData2);
    EXPECT_FALSE(mapData != mapData2);

    mapData.setTargetTypeRef(testTypeId2);

    EXPECT_FALSE(mapData == mapData2);
    EXPECT_TRUE(mapData != mapData2);

    mapData2.setTargetTypeRef(testTypeId2);

    EXPECT_TRUE(mapData == mapData2);
    EXPECT_FALSE(mapData != mapData2);

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_FALSE(mapData == mapData2);
    EXPECT_TRUE(mapData != mapData2);

    mapData2.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_TRUE(mapData == mapData2);
    EXPECT_FALSE(mapData != mapData2);

    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    mapData2.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapData == mapData2);
    EXPECT_TRUE(mapData != mapData2);
}

TEST(MapDataTest, getHash) {
    // NOTE: There's a small chance that this test will fail due a hash collision.

    babelwires::MapData mapData;
    babelwires::MapData mapData2;

    EXPECT_EQ(mapData.getHash(), mapData2.getHash());

    mapData.setSourceTypeRef(testTypeId1);

    EXPECT_NE(mapData.getHash(), mapData2.getHash());

    mapData2.setSourceTypeRef(testTypeId1);

    EXPECT_EQ(mapData.getHash(), mapData2.getHash());

    mapData.setTargetTypeRef(testTypeId2);

    EXPECT_NE(mapData.getHash(), mapData2.getHash());

    mapData2.setTargetTypeRef(testTypeId2);

    EXPECT_EQ(mapData.getHash(), mapData2.getHash());

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_NE(mapData.getHash(), mapData2.getHash());

    mapData2.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_EQ(mapData.getHash(), mapData2.getHash());

    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    mapData2.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_NE(mapData.getHash(), mapData2.getHash());
}

TEST(MapDataTest, isInvalid_validMap) {
    babelwires::MapData mapData;

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_TRUE(mapData.isValid(typeSystem));

    // This will invalidate the previous entry, since a fallback is only permitted at the end.
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapData.isValid(typeSystem));
}

TEST(MapDataTest, isInvalid_outOfPlaceFallback) {
    babelwires::MapData mapData;

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapData.isValid(typeSystem));
}

TEST(MapDataTest, isInvalid_noFallback) {
    babelwires::MapData mapData;

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapData.setSourceTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_FALSE(mapData.isValid(typeSystem));
}

TEST(MapDataTest, isValid_typeMismatch) {
    babelwires::MapData mapData;

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testUtils::TestType::getThisIdentifier());
    mapData.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapData.isValid(typeSystem));
}

TEST(MapDataTest, serializationTest) {
    std::string serializedContents;
    {
        babelwires::MapData mapData;
        mapData.setSourceTypeRef(testTypeId1);
        mapData.setTargetTypeRef(testTypeId2);
    
        babelwires::IdentifierRegistryScope identifierRegistry;
        babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<testUtils::TestType>();  

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        auto entryData = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier());
        auto entrySourceValue = std::make_unique<testUtils::TestValue>();
        entrySourceValue->m_value = "test mapData serialization";
        entryData->setSourceValue(std::move(entrySourceValue));
        mapData.emplaceBack(std::move(entryData));
        mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(mapData);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::MapData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_sourceTypeRef, testTypeId1);
    EXPECT_EQ(dataPtr->m_targetTypeRef, testTypeId2);
    EXPECT_EQ(dataPtr->getNumMapEntries(), 2);
    EXPECT_EQ(dataPtr->getMapEntry(0).getKind(), babelwires::MapEntryData::Kind::OneToOne);
    const auto *const sourceValue = dataPtr->getMapEntry(0).as<babelwires::OneToOneMapEntryData>()->getSourceValue();
    ASSERT_NE(sourceValue, nullptr);
    const auto *const sourceAsTestValue = sourceValue->as<testUtils::TestValue>();
    ASSERT_NE(sourceAsTestValue, nullptr);
    EXPECT_EQ(sourceAsTestValue->m_value, "test mapData serialization");

    EXPECT_EQ(dataPtr->getMapEntry(1).getKind(), babelwires::MapEntryData::Kind::AllToSame);
}


TEST(MapDataTest, cloneTest) {
    babelwires::MapData mapData;
    mapData.setSourceTypeRef(testTypeId1);
    mapData.setTargetTypeRef(testTypeId2);

    babelwires::IdentifierRegistryScope identifierRegistry;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    // Note: We want to be able to clone when entries do not match the types, as in this case.
    auto entryData = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier());
    auto entrySourceValue = std::make_unique<testUtils::TestValue>();
    auto entryDataPtr = entryData.get();
    auto entrySourceValuePtr = entrySourceValue.get();
    entrySourceValue->m_value = "test mapData serialization";
    entryData->setSourceValue(std::move(entrySourceValue));
    mapData.emplaceBack(std::move(entryData));
    mapData.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    auto cloneMapData = mapData.clone();

    ASSERT_NE(cloneMapData, nullptr);
    EXPECT_EQ(cloneMapData->m_sourceTypeRef, testTypeId1);
    EXPECT_EQ(cloneMapData->m_targetTypeRef, testTypeId2);
    EXPECT_EQ(cloneMapData->getNumMapEntries(), 2);
    EXPECT_EQ(cloneMapData->getMapEntry(0).getKind(), babelwires::MapEntryData::Kind::OneToOne);
    const auto *const clonedEntryData = cloneMapData->getMapEntry(0).as<babelwires::OneToOneMapEntryData>();
    ASSERT_NE(clonedEntryData, nullptr);
    EXPECT_NE(clonedEntryData, entryDataPtr);
    EXPECT_EQ(*clonedEntryData, *entryDataPtr);
    EXPECT_EQ(cloneMapData->getMapEntry(1).getKind(), babelwires::MapEntryData::Kind::AllToSame);
}

