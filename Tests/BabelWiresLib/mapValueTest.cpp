#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::PrimitiveTypeId testTypeId1 = "TestType1";
    const babelwires::PrimitiveTypeId testTypeId2 = "TestType2";
} // namespace

TEST(MapValueTest, types) {
    babelwires::MapValue mapValue;

    EXPECT_EQ(mapValue.getSourceTypeRef(), babelwires::TypeRef());
    EXPECT_EQ(mapValue.getTargetTypeRef(), babelwires::TypeRef());

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);
    EXPECT_EQ(mapValue.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapValue.getTargetTypeRef(), testTypeId2);
}

TEST(MapValueTest, entries) {
    babelwires::MapValue mapValue;

    EXPECT_EQ(mapValue.getNumMapEntries(), 0);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    EXPECT_EQ(mapValue.getNumMapEntries(), 1);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    EXPECT_EQ(mapValue.getNumMapEntries(), 2);
}

TEST(MapValueTest, copyConstruction) {
    babelwires::MapValue mapValue;

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    babelwires::MapValue mapValue2 = mapValue;
    EXPECT_EQ(mapValue2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapValue2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapValue2.getNumMapEntries(), 1);
    // Ensure mapEntries are not shared.
    EXPECT_NE(&mapValue.getMapEntry(0), &mapValue2.getMapEntry(0));
}

TEST(MapValueTest, moveConstruction) {
    babelwires::MapValue mapValue;

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    auto mapEntry = &mapValue.getMapEntry(0);

    babelwires::MapValue mapValue2 = std::move(mapValue);
    EXPECT_EQ(mapValue2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapValue2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapValue2.getNumMapEntries(), 1);
    // Ensure mapEntries are moved.
    EXPECT_EQ(&mapValue2.getMapEntry(0), mapEntry);
}

TEST(MapValueTest, copyAssignment) {
    babelwires::MapValue mapValue;

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    babelwires::MapValue mapValue2;
    mapValue2 = mapValue;
    EXPECT_EQ(mapValue2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapValue2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapValue2.getNumMapEntries(), 1);
    // Ensure mapEntries are not shared.
    EXPECT_NE(&mapValue.getMapEntry(0), &mapValue2.getMapEntry(0));
}

TEST(MapValueTest, moveAssignment) {
    babelwires::MapValue mapValue;

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    auto mapEntry = &mapValue.getMapEntry(0);

    babelwires::MapValue mapValue2;
    mapValue2 = std::move(mapValue);
    EXPECT_EQ(mapValue2.getSourceTypeRef(), testTypeId1);
    EXPECT_EQ(mapValue2.getTargetTypeRef(), testTypeId2);
    EXPECT_EQ(mapValue2.getNumMapEntries(), 1);
    // Ensure mapEntries are moved.
    EXPECT_EQ(&mapValue2.getMapEntry(0), mapEntry);
}

TEST(MapValueTest, equality) {
    babelwires::MapValue mapValue;
    babelwires::MapValue mapValue2;

    EXPECT_TRUE(mapValue == mapValue2);
    EXPECT_FALSE(mapValue != mapValue2);

    mapValue.setSourceTypeRef(testTypeId1);

    EXPECT_FALSE(mapValue == mapValue2);
    EXPECT_TRUE(mapValue != mapValue2);

    mapValue2.setSourceTypeRef(testTypeId1);

    EXPECT_TRUE(mapValue == mapValue2);
    EXPECT_FALSE(mapValue != mapValue2);

    mapValue.setTargetTypeRef(testTypeId2);

    EXPECT_FALSE(mapValue == mapValue2);
    EXPECT_TRUE(mapValue != mapValue2);

    mapValue2.setTargetTypeRef(testTypeId2);

    EXPECT_TRUE(mapValue == mapValue2);
    EXPECT_FALSE(mapValue != mapValue2);

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));

    EXPECT_FALSE(mapValue == mapValue2);
    EXPECT_TRUE(mapValue != mapValue2);

    mapValue2.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));

    EXPECT_TRUE(mapValue == mapValue2);
    EXPECT_FALSE(mapValue != mapValue2);

    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));
    mapValue2.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapValue == mapValue2);
    EXPECT_TRUE(mapValue != mapValue2);
}

TEST(MapValueTest, getHash) {
    // NOTE: There's a small chance that this test will fail due a hash collision.

    babelwires::MapValue mapValue;
    babelwires::MapValue mapValue2;

    EXPECT_EQ(mapValue.getHash(), mapValue2.getHash());

    mapValue.setSourceTypeRef(testTypeId1);

    EXPECT_NE(mapValue.getHash(), mapValue2.getHash());

    mapValue2.setSourceTypeRef(testTypeId1);

    EXPECT_EQ(mapValue.getHash(), mapValue2.getHash());

    mapValue.setTargetTypeRef(testTypeId2);

    EXPECT_NE(mapValue.getHash(), mapValue2.getHash());

    mapValue2.setTargetTypeRef(testTypeId2);

    EXPECT_EQ(mapValue.getHash(), mapValue2.getHash());

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();

    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));

    EXPECT_NE(mapValue.getHash(), mapValue2.getHash());

    mapValue2.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));

    EXPECT_EQ(mapValue.getHash(), mapValue2.getHash());

    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));
    mapValue2.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_NE(mapValue.getHash(), mapValue2.getHash());
}

TEST(MapValueTest, isInvalid_validMap) {
    babelwires::MapValue mapValue;

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());
    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_TRUE(mapValue.isValid(typeSystem));

    // This will invalidate the previous entry, since a fallback is only permitted at the end.
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapValue.isValid(typeSystem));
}

TEST(MapValueTest, isInvalid_outOfPlaceFallback) {
    babelwires::MapValue mapValue;

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());
    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapValue.isValid(typeSystem));
}

TEST(MapValueTest, isInvalid_noFallback) {
    babelwires::MapValue mapValue;

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapValue.setSourceTypeRef(testUtils::TestType::getThisType());
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());
    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));

    EXPECT_FALSE(mapValue.isValid(typeSystem));
}

TEST(MapValueTest, isValid_typeMismatch) {
    babelwires::MapValue mapValue;

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testUtils::TestType::getThisType());
    mapValue.emplaceBack(std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType()));
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    EXPECT_FALSE(mapValue.isValid(typeSystem));
}

TEST(MapValueTest, serializationTest) {
    std::string serializedContents;
    {
        babelwires::MapValue mapValue;
        mapValue.setSourceTypeRef(testTypeId1);
        mapValue.setTargetTypeRef(testTypeId2);
    
        babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<testUtils::TestType>();  

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        auto entryData = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType());
        testUtils::TestValue entrySourceValue;
        entrySourceValue.m_value = "test mapValue serialization";
        entryData->setSourceValue(entrySourceValue);
        mapValue.emplaceBack(std::move(entryData));
        mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(mapValue);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::MapValue>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_sourceTypeRef, testTypeId1);
    EXPECT_EQ(dataPtr->m_targetTypeRef, testTypeId2);
    EXPECT_EQ(dataPtr->getNumMapEntries(), 2);
    EXPECT_EQ(dataPtr->getMapEntry(0).getKind(), babelwires::MapEntryData::Kind::One21);
    const babelwires::EditableValueHolder& sourceValue = dataPtr->getMapEntry(0).as<babelwires::OneToOneMapEntryData>()->getSourceValue();
    ASSERT_NE(sourceValue, nullptr);
    const auto *const sourceAsTestValue = sourceValue->as<testUtils::TestValue>();
    ASSERT_NE(sourceAsTestValue, nullptr);
    EXPECT_EQ(sourceAsTestValue->m_value, "test mapValue serialization");

    EXPECT_EQ(dataPtr->getMapEntry(1).getKind(), babelwires::MapEntryData::Kind::All2Sm);
}


TEST(MapValueTest, cloneTest) {
    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(testTypeId1);
    mapValue.setTargetTypeRef(testTypeId2);

    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    // Note: We want to be able to clone when entries do not match the types, as in this case.
    auto entryData = std::make_unique<babelwires::OneToOneMapEntryData>(typeSystem, testUtils::TestType::getThisType(), testUtils::TestType::getThisType());
    auto entryDataPtr = entryData.get();
    testUtils::TestValue entrySourceValue;
    entrySourceValue.m_value = "test mapValue serialization";
    entryData->setSourceValue(entrySourceValue);
    mapValue.emplaceBack(std::move(entryData));
    mapValue.emplaceBack(std::make_unique<babelwires::AllToSameFallbackMapEntryData>());

    auto cloneMapValue = mapValue.clone();

    ASSERT_NE(cloneMapValue, nullptr);
    EXPECT_EQ(cloneMapValue->m_sourceTypeRef, testTypeId1);
    EXPECT_EQ(cloneMapValue->m_targetTypeRef, testTypeId2);
    EXPECT_EQ(cloneMapValue->getNumMapEntries(), 2);
    EXPECT_EQ(cloneMapValue->getMapEntry(0).getKind(), babelwires::MapEntryData::Kind::One21);
    const auto *const clonedEntryData = cloneMapValue->getMapEntry(0).as<babelwires::OneToOneMapEntryData>();
    ASSERT_NE(clonedEntryData, nullptr);
    EXPECT_NE(clonedEntryData, entryDataPtr);
    EXPECT_EQ(*clonedEntryData, *entryDataPtr);
    EXPECT_EQ(cloneMapValue->getMapEntry(1).getKind(), babelwires::MapEntryData::Kind::All2Sm);
}

