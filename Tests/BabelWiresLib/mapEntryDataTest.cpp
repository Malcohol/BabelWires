#include <gtest/gtest.h>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::LongId testTypeId1 = "TestType1";
    const babelwires::LongId testTypeId2 = "TestType2";
} // namespace

TEST(MapEntryDataTest, isFallback) {
    EXPECT_FALSE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::OneToOne));
    EXPECT_TRUE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::AllToOne));
    EXPECT_TRUE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::AllToSame));
}

TEST(MapEntryDataTest, getKindName) {
    const auto oneToOneName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::OneToOne);
    const auto allToOneName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::AllToOne);
    const auto allToSameName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::AllToSame);

    EXPECT_GT(oneToOneName.length(), 0);
    EXPECT_GT(allToOneName.length(), 0);
    EXPECT_GT(allToSameName.length(), 0);

    EXPECT_NE(oneToOneName, allToOneName);
    EXPECT_NE(oneToOneName, allToSameName);
    EXPECT_NE(allToOneName, allToSameName);
}

TEST(MapEntryDataTest, create) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    const auto oneToOne = babelwires::MapEntryData::create(typeSystem, testUtils::TestType::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier(),
                                                           babelwires::MapEntryData::Kind::OneToOne);
    const auto allToOne = babelwires::MapEntryData::create(typeSystem, testUtils::TestType::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier(),
                                                           babelwires::MapEntryData::Kind::AllToOne);
    // source == target for allToSame
    const auto allToSame = babelwires::MapEntryData::create(typeSystem, testUtils::TestType::getThisIdentifier(),
                                                            testUtils::TestType::getThisIdentifier(),
                                                            babelwires::MapEntryData::Kind::AllToSame);

    EXPECT_EQ(oneToOne->getKind(), babelwires::MapEntryData::Kind::OneToOne);
    EXPECT_EQ(allToOne->getKind(), babelwires::MapEntryData::Kind::AllToOne);
    EXPECT_EQ(allToSame->getKind(), babelwires::MapEntryData::Kind::AllToSame);

    EXPECT_TRUE(oneToOne->validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), false));
    EXPECT_TRUE(allToOne->validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));
    EXPECT_TRUE(allToSame->validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                    testUtils::TestType::getThisIdentifier(), true));

    const auto* const oneToOneEntryData = oneToOne->as<babelwires::OneToOneMapEntryData>();
    const auto* const allToOneEntryData = allToOne->as<babelwires::AllToOneFallbackMapEntryData>();
    const auto* const allToSameEntryData = allToSame->as<babelwires::AllToSameFallbackMapEntryData>();

    EXPECT_NE(oneToOneEntryData, nullptr);
    EXPECT_NE(allToOneEntryData, nullptr);
    EXPECT_NE(allToSameEntryData, nullptr);

    EXPECT_NE(oneToOneEntryData->getSourceValue()->as<testUtils::TestValue>(), nullptr);
    EXPECT_NE(oneToOneEntryData->getTargetValue()->as<babelwires::EnumValue>(), nullptr);
    EXPECT_NE(allToOneEntryData->getTargetValue()->as<babelwires::EnumValue>(), nullptr);
}

TEST(MapEntryDataTest, equalityByKind) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    const babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                                    testUtils::TestEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestEnum::getThisIdentifier());
    babelwires::AllToSameFallbackMapEntryData allToSame;

    EXPECT_EQ(oneToOne, oneToOne);
    EXPECT_EQ(allToOne, allToOne);
    EXPECT_EQ(allToSame, allToSame);

    EXPECT_NE(oneToOne, allToOne);
    EXPECT_NE(oneToOne, allToSame);
    EXPECT_NE(allToOne, allToSame);
}

TEST(MapEntryDataTest, oneToOneEqualitySameTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    EXPECT_EQ(oneToOneA, oneToOneB);

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "equality test";

    oneToOneA.setSourceValue(sourceValue.clone());
    EXPECT_NE(oneToOneA, oneToOneB);

    oneToOneB.setSourceValue(sourceValue.clone());
    EXPECT_EQ(oneToOneA, oneToOneB);

    oneToOneA.setTargetValue(sourceValue.clone());
    EXPECT_NE(oneToOneA, oneToOneB);

    oneToOneB.setTargetValue(sourceValue.clone());
    EXPECT_EQ(oneToOneA, oneToOneB);
}

TEST(MapEntryDataTest, oneToOneEqualityDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneC(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneD(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                               testUtils::TestEnum::getThisIdentifier());

    EXPECT_NE(oneToOneA, oneToOneB);
    EXPECT_NE(oneToOneA, oneToOneC);
    EXPECT_NE(oneToOneA, oneToOneD);
    EXPECT_NE(oneToOneB, oneToOneC);
    EXPECT_NE(oneToOneB, oneToOneD);
    EXPECT_NE(oneToOneC, oneToOneD);

    EXPECT_NE(oneToOneB, oneToOneA);
    EXPECT_NE(oneToOneC, oneToOneA);
    EXPECT_NE(oneToOneD, oneToOneA);
    EXPECT_NE(oneToOneC, oneToOneB);
    EXPECT_NE(oneToOneD, oneToOneB);
    EXPECT_NE(oneToOneD, oneToOneC);
}

TEST(MapEntryDataTest, allToOneEqualitySameTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testUtils::TestType::getThisIdentifier());

    EXPECT_EQ(allToOneA, allToOneB);

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "equality test";

    allToOneA.setTargetValue(sourceValue.clone());
    EXPECT_NE(allToOneA, allToOneB);

    allToOneB.setTargetValue(sourceValue.clone());
    EXPECT_EQ(allToOneA, allToOneB);
}

TEST(MapEntryDataTest, allToOneEqualityDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testUtils::TestEnum::getThisIdentifier());

    EXPECT_NE(allToOneA, allToOneB);
    EXPECT_NE(allToOneB, allToOneA);
}

TEST(MapEntryDataTest, hashByKind) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    const babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                                    testUtils::TestEnum::getThisIdentifier());
    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestEnum::getThisIdentifier());
    babelwires::AllToSameFallbackMapEntryData allToSame;

    const auto oneToOneHash = oneToOne.getHash();
    const auto allToOneHash = allToOne.getHash();
    const auto allToSameHash = allToSame.getHash();

    EXPECT_NE(oneToOneHash, allToOneHash);
    EXPECT_NE(oneToOneHash, allToSameHash);
    EXPECT_NE(allToOneHash, allToSameHash);
}

TEST(MapEntryDataTest, oneToOneHashSameTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "equality test";

    oneToOneA.setSourceValue(sourceValue.clone());
    EXPECT_NE(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneB.setSourceValue(sourceValue.clone());
    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneA.setTargetValue(sourceValue.clone());
    EXPECT_NE(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneB.setTargetValue(sourceValue.clone());
    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());
}

TEST(MapEntryDataTest, oneToOneHashDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, testUtils::TestType::getThisIdentifier(),
                                               testUtils::TestEnum::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneC(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                               testUtils::TestType::getThisIdentifier());

    babelwires::OneToOneMapEntryData oneToOneD(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                               testUtils::TestEnum::getThisIdentifier());

    EXPECT_NE(oneToOneA.getHash(), oneToOneB.getHash());
    EXPECT_NE(oneToOneA.getHash(), oneToOneC.getHash());
    EXPECT_NE(oneToOneA.getHash(), oneToOneD.getHash());
    EXPECT_NE(oneToOneB.getHash(), oneToOneC.getHash());
    EXPECT_NE(oneToOneB.getHash(), oneToOneD.getHash());
    EXPECT_NE(oneToOneC.getHash(), oneToOneD.getHash());

    EXPECT_NE(oneToOneB.getHash(), oneToOneA.getHash());
    EXPECT_NE(oneToOneC.getHash(), oneToOneA.getHash());
    EXPECT_NE(oneToOneD.getHash(), oneToOneA.getHash());
    EXPECT_NE(oneToOneC.getHash(), oneToOneB.getHash());
    EXPECT_NE(oneToOneD.getHash(), oneToOneB.getHash());
    EXPECT_NE(oneToOneD.getHash(), oneToOneC.getHash());
}

TEST(MapEntryDataTest, allToOneHashSameTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testUtils::TestType::getThisIdentifier());

    EXPECT_EQ(allToOneA.getHash(), allToOneB.getHash());

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "equality test";

    allToOneA.setTargetValue(sourceValue.clone());
    EXPECT_NE(allToOneA.getHash(), allToOneB.getHash());

    allToOneB.setTargetValue(sourceValue.clone());
    EXPECT_EQ(allToOneA.getHash(), allToOneB.getHash());
}

TEST(MapEntryDataTest, allToOneHashDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, testUtils::TestType::getThisIdentifier());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testUtils::TestEnum::getThisIdentifier());

    EXPECT_NE(allToOneA.getHash(), allToOneB.getHash());
    EXPECT_NE(allToOneB.getHash(), allToOneA.getHash());
}

TEST(MapEntryDataTest, oneToOneValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestEnum::getThisIdentifier());

    EXPECT_FALSE(oneToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                  testUtils::TestType::getThisIdentifier(), false));
    EXPECT_TRUE(oneToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), false));
    EXPECT_FALSE(oneToOne.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestType::getThisIdentifier(), false));
    EXPECT_FALSE(oneToOne.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), false));

    EXPECT_FALSE(oneToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));
}

TEST(MapEntryDataTest, allToOneValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestType::getThisIdentifier());

    EXPECT_TRUE(allToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                  testUtils::TestType::getThisIdentifier(), true));
    EXPECT_FALSE(allToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));
    EXPECT_TRUE(allToOne.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestType::getThisIdentifier(), true));
    EXPECT_FALSE(allToOne.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));

    EXPECT_FALSE(allToOne.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestType::getThisIdentifier(), false));
}

TEST(MapEntryDataTest, allToSameValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToSameFallbackMapEntryData allToSame;

    EXPECT_TRUE(allToSame.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                  testUtils::TestType::getThisIdentifier(), true));
    EXPECT_FALSE(allToSame.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));
    EXPECT_FALSE(allToSame.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestType::getThisIdentifier(), true));
    EXPECT_TRUE(allToSame.validate(typeSystem, testUtils::TestEnum::getThisIdentifier(),
                                   testUtils::TestEnum::getThisIdentifier(), true));

    EXPECT_FALSE(allToSame.validate(typeSystem, testUtils::TestType::getThisIdentifier(),
                                   testUtils::TestType::getThisIdentifier(), false));
}

TEST(MapEntryDataTest, oneToOneGetAndSetValues) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestEnum::getThisIdentifier());

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "source";

    oneToOne.setSourceValue(sourceValue.clone());
    const auto sourceValueFromData = oneToOne.getSourceValue()->as<testUtils::TestValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->m_value, sourceValue.m_value);

    babelwires::EnumValue targetValue;
    targetValue.set("Foo");
    oneToOne.setTargetValue(targetValue.clone());
    const auto targetValueFromData = oneToOne.getTargetValue()->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), targetValue.get());
}

TEST(MapEntryDataTest, allToOneGetAndSetValues) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestType::getThisIdentifier());

    testUtils::TestValue targetValue;
    targetValue.m_value = "source";

    allToOne.setTargetValue(targetValue.clone());
    const auto targetValueFromData = allToOne.getTargetValue()->as<testUtils::TestValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->m_value, targetValue.m_value);
}

TEST(MapEntryDataTest, oneToOneSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<testUtils::TestType>();  
        typeSystem.addEntry<testUtils::TestEnum>();

        babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                              testUtils::TestEnum::getThisIdentifier());

        testUtils::TestValue sourceValue;
        sourceValue.m_value = "test serialization";

        oneToOne.setSourceValue(sourceValue.clone());

        babelwires::EnumValue targetValue;
        targetValue.set("Foo");
        oneToOne.setTargetValue(targetValue.clone());

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(oneToOne);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::OneToOneMapEntryData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    const auto *const sourceValue = dataPtr->getSourceValue();
    const auto *const targetValue = dataPtr->getTargetValue();
    ASSERT_NE(sourceValue, nullptr);
    ASSERT_NE(targetValue, nullptr);

    const auto *const sourceValueFromData = sourceValue->as<testUtils::TestValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->m_value, "test serialization");

    const auto *const targetValueFromData = targetValue->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "Foo");
}

TEST(MapEntryDataTest, allToOneSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<testUtils::TestType>();  

        babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestType::getThisIdentifier());

        testUtils::TestValue targetValue;
        targetValue.m_value = "test serialization";

        allToOne.setTargetValue(targetValue.clone());

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(allToOne);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::AllToOneFallbackMapEntryData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    const auto *const targetValue = dataPtr->getTargetValue();
    ASSERT_NE(targetValue, nullptr);

    const auto *const targetValueFromData = targetValue->as<testUtils::TestValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->m_value, "test serialization");
}

TEST(MapEntryDataTest, allToSameSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<testUtils::TestType>();  

        babelwires::AllToSameFallbackMapEntryData allToSame;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(allToSame);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::AllToSameFallbackMapEntryData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
}


TEST(MapEntryDataTest, oneToOneClone) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  
    typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, testUtils::TestType::getThisIdentifier(),
                                            testUtils::TestEnum::getThisIdentifier());

    testUtils::TestValue sourceValue;
    sourceValue.m_value = "test serialization";

    oneToOne.setSourceValue(sourceValue.clone());

    babelwires::EnumValue targetValue;
    targetValue.set("Foo");
    oneToOne.setTargetValue(targetValue.clone());

    auto dataPtr = oneToOne.clone();

    ASSERT_NE(dataPtr, nullptr);
    const auto *const sourceValueInClone = dataPtr->getSourceValue();
    const auto *const targetValueInClone = dataPtr->getTargetValue();
    ASSERT_NE(sourceValueInClone, nullptr);
    ASSERT_NE(targetValueInClone, nullptr);

    const auto *const sourceValueFromData = sourceValueInClone->as<testUtils::TestValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->m_value, "test serialization");

    const auto *const targetValueFromData = targetValueInClone->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "Foo");
}

TEST(MapEntryDataTest, allToOneClone) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testUtils::TestType::getThisIdentifier());

    testUtils::TestValue targetValue;
    targetValue.m_value = "test serialization";

    allToOne.setTargetValue(targetValue.clone());

    auto dataPtr = allToOne.clone();

    ASSERT_NE(dataPtr, nullptr);
    const auto *const targetValueInClone = dataPtr->getTargetValue();
    ASSERT_NE(targetValueInClone, nullptr);

    const auto *const targetValueFromData = targetValueInClone->as<testUtils::TestValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->m_value, "test serialization");
}

TEST(MapEntryDataTest, allToSameClone) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<testUtils::TestType>();  

    babelwires::AllToSameFallbackMapEntryData allToSame;

    auto dataPtr = allToSame.clone();

    ASSERT_NE(dataPtr, nullptr);
}
