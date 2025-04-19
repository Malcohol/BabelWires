#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/TestUtils/testLog.hpp>

namespace {
    const babelwires::LongId testTypeId1 = "TestType1";
    const babelwires::LongId testTypeId2 = "TestType2";
} // namespace

TEST(MapEntryDataTest, isFallback) {
    EXPECT_FALSE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::One21));
    EXPECT_TRUE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::All21));
    EXPECT_TRUE(babelwires::MapEntryData::isFallback(babelwires::MapEntryData::Kind::All2Sm));
}

TEST(MapEntryDataTest, getKindName) {
    const auto oneToOneName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::One21);
    const auto allToOneName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::All21);
    const auto allToSameName = babelwires::MapEntryData::getKindName(babelwires::MapEntryData::Kind::All2Sm);

    EXPECT_GT(oneToOneName.length(), 0);
    EXPECT_GT(allToOneName.length(), 0);
    EXPECT_GT(allToSameName.length(), 0);

    EXPECT_NE(oneToOneName, allToOneName);
    EXPECT_NE(oneToOneName, allToSameName);
    EXPECT_NE(allToOneName, allToSameName);
}

TEST(MapEntryDataTest, create) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    const auto oneToOne = babelwires::MapEntryData::create(typeSystem, babelwires::StringType::getThisType(),
                                                           testDomain::TestEnum::getThisType(),
                                                           babelwires::MapEntryData::Kind::One21);
    const auto allToOne = babelwires::MapEntryData::create(typeSystem, babelwires::StringType::getThisType(),
                                                           testDomain::TestEnum::getThisType(),
                                                           babelwires::MapEntryData::Kind::All21);
    // source == target for allToSame
    const auto allToSame = babelwires::MapEntryData::create(typeSystem, babelwires::StringType::getThisType(),
                                                            babelwires::StringType::getThisType(),
                                                            babelwires::MapEntryData::Kind::All2Sm);

    EXPECT_EQ(oneToOne->getKind(), babelwires::MapEntryData::Kind::One21);
    EXPECT_EQ(allToOne->getKind(), babelwires::MapEntryData::Kind::All21);
    EXPECT_EQ(allToSame->getKind(), babelwires::MapEntryData::Kind::All2Sm);

    EXPECT_TRUE(oneToOne->validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), false));
    EXPECT_TRUE(allToOne->validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));
    EXPECT_TRUE(allToSame->validate(typeSystem, babelwires::StringType::getThisType(),
                                    babelwires::StringType::getThisType(), true));

    const auto* const oneToOneEntryData = oneToOne->as<babelwires::OneToOneMapEntryData>();
    const auto* const allToOneEntryData = allToOne->as<babelwires::AllToOneFallbackMapEntryData>();
    const auto* const allToSameEntryData = allToSame->as<babelwires::AllToSameFallbackMapEntryData>();

    EXPECT_NE(oneToOneEntryData, nullptr);
    EXPECT_NE(allToOneEntryData, nullptr);
    EXPECT_NE(allToSameEntryData, nullptr);

    EXPECT_NE(oneToOneEntryData->getSourceValue()->as<babelwires::StringValue>(), nullptr);
    EXPECT_NE(oneToOneEntryData->getTargetValue()->as<babelwires::EnumValue>(), nullptr);
    EXPECT_NE(allToOneEntryData->getTargetValue()->as<babelwires::EnumValue>(), nullptr);
}

TEST(MapEntryDataTest, equalityByKind) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                                    testDomain::TestEnum::getThisType());
    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testDomain::TestEnum::getThisType());
    babelwires::AllToSameFallbackMapEntryData allToSame;

    EXPECT_EQ(oneToOne, oneToOne);
    EXPECT_EQ(allToOne, allToOne);
    EXPECT_EQ(allToSame, allToSame);

    //EXPECT_NE(oneToOne, allToOne);
    //EXPECT_NE(oneToOne, allToSame);
    //EXPECT_NE(allToOne, allToSame);
}

TEST(MapEntryDataTest, oneToOneEqualitySameTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    EXPECT_EQ(oneToOneA, oneToOneB);

    babelwires::StringValue sourceValue;
    sourceValue.set("equality test");

    oneToOneA.setSourceValue(sourceValue);
    EXPECT_NE(oneToOneA, oneToOneB);

    oneToOneB.setSourceValue(sourceValue);
    EXPECT_EQ(oneToOneA, oneToOneB);

    oneToOneA.setTargetValue(sourceValue);
    EXPECT_NE(oneToOneA, oneToOneB);

    oneToOneB.setTargetValue(sourceValue);
    EXPECT_EQ(oneToOneA, oneToOneB);
}

TEST(MapEntryDataTest, oneToOneEqualityDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, babelwires::StringType::getThisType(),
                                               testDomain::TestEnum::getThisType());

    babelwires::OneToOneMapEntryData oneToOneC(typeSystem, testDomain::TestEnum::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneD(typeSystem, testDomain::TestEnum::getThisType(),
                                               testDomain::TestEnum::getThisType());

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
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, babelwires::StringType::getThisType());

    EXPECT_EQ(allToOneA, allToOneB);

    babelwires::StringValue sourceValue;
    sourceValue.set("equality test");

    allToOneA.setTargetValue(sourceValue);
    EXPECT_NE(allToOneA, allToOneB);

    allToOneB.setTargetValue(sourceValue);
    EXPECT_EQ(allToOneA, allToOneB);
}

TEST(MapEntryDataTest, allToOneEqualityDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testDomain::TestEnum::getThisType());

    EXPECT_NE(allToOneA, allToOneB);
    EXPECT_NE(allToOneB, allToOneA);
}

TEST(MapEntryDataTest, hashByKind) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    const babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                                    testDomain::TestEnum::getThisType());
    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, testDomain::TestEnum::getThisType());
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
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());

    babelwires::StringValue sourceValue;
    sourceValue.set("equality test");

    oneToOneA.setSourceValue(sourceValue);
    EXPECT_NE(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneB.setSourceValue(sourceValue);
    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneA.setTargetValue(sourceValue);
    EXPECT_NE(oneToOneA.getHash(), oneToOneB.getHash());

    oneToOneB.setTargetValue(sourceValue);
    EXPECT_EQ(oneToOneA.getHash(), oneToOneB.getHash());
}

TEST(MapEntryDataTest, oneToOneHashDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOneA(typeSystem, babelwires::StringType::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneB(typeSystem, babelwires::StringType::getThisType(),
                                               testDomain::TestEnum::getThisType());

    babelwires::OneToOneMapEntryData oneToOneC(typeSystem, testDomain::TestEnum::getThisType(),
                                               babelwires::StringType::getThisType());

    babelwires::OneToOneMapEntryData oneToOneD(typeSystem, testDomain::TestEnum::getThisType(),
                                               testDomain::TestEnum::getThisType());

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
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, babelwires::StringType::getThisType());

    EXPECT_EQ(allToOneA.getHash(), allToOneB.getHash());

    babelwires::StringValue sourceValue;
    sourceValue.set("equality test");

    allToOneA.setTargetValue(sourceValue);
    EXPECT_NE(allToOneA.getHash(), allToOneB.getHash());

    allToOneB.setTargetValue(sourceValue);
    EXPECT_EQ(allToOneA.getHash(), allToOneB.getHash());
}

TEST(MapEntryDataTest, allToOneHashDifferentTypes) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOneA(typeSystem, babelwires::StringType::getThisType());

    babelwires::AllToOneFallbackMapEntryData allToOneB(typeSystem, testDomain::TestEnum::getThisType());

    EXPECT_NE(allToOneA.getHash(), allToOneB.getHash());
    EXPECT_NE(allToOneB.getHash(), allToOneA.getHash());
}

TEST(MapEntryDataTest, oneToOneValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestEnum::getThisType());

    EXPECT_FALSE(oneToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                  babelwires::StringType::getThisType(), false));
    EXPECT_TRUE(oneToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), false));
    EXPECT_FALSE(oneToOne.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   babelwires::StringType::getThisType(), false));
    EXPECT_FALSE(oneToOne.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   testDomain::TestEnum::getThisType(), false));

    EXPECT_FALSE(oneToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));
}

TEST(MapEntryDataTest, allToOneValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, babelwires::StringType::getThisType());

    EXPECT_TRUE(allToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                  babelwires::StringType::getThisType(), true));
    EXPECT_FALSE(allToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));
    EXPECT_TRUE(allToOne.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   babelwires::StringType::getThisType(), true));
    EXPECT_FALSE(allToOne.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));

    EXPECT_FALSE(allToOne.validate(typeSystem, babelwires::StringType::getThisType(),
                                   babelwires::StringType::getThisType(), false));
}

TEST(MapEntryDataTest, allToSameValidate) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToSameFallbackMapEntryData allToSame;

    EXPECT_TRUE(allToSame.validate(typeSystem, babelwires::StringType::getThisType(),
                                  babelwires::StringType::getThisType(), true));
    EXPECT_FALSE(allToSame.validate(typeSystem, babelwires::StringType::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));
    EXPECT_FALSE(allToSame.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   babelwires::StringType::getThisType(), true));
    EXPECT_TRUE(allToSame.validate(typeSystem, testDomain::TestEnum::getThisType(),
                                   testDomain::TestEnum::getThisType(), true));

    EXPECT_FALSE(allToSame.validate(typeSystem, babelwires::StringType::getThisType(),
                                   babelwires::StringType::getThisType(), false));
}

TEST(MapEntryDataTest, oneToOneGetAndSetValues) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestEnum::getThisType());

    babelwires::StringValue sourceValue;
    sourceValue.set("source");

    oneToOne.setSourceValue(sourceValue);
    const auto sourceValueFromData = oneToOne.getSourceValue()->as<babelwires::StringValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->get(), sourceValue.get());

    babelwires::EnumValue targetValue;
    targetValue.set("Foo");
    oneToOne.setTargetValue(targetValue);
    const auto targetValueFromData = oneToOne.getTargetValue()->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), targetValue.get());
}

TEST(MapEntryDataTest, allToOneGetAndSetValues) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, babelwires::StringType::getThisType());

    babelwires::StringValue targetValue;
    targetValue.set("source");

    allToOne.setTargetValue(targetValue);
    const auto targetValueFromData = allToOne.getTargetValue()->as<babelwires::StringValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), targetValue.get());
}

TEST(MapEntryDataTest, oneToOneSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<babelwires::StringType>();  
        typeSystem.addEntry<testDomain::TestEnum>();

        babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                              testDomain::TestEnum::getThisType());

        babelwires::StringValue sourceValue;
        sourceValue.set("test serialization");

        oneToOne.setSourceValue(sourceValue);

        babelwires::EnumValue targetValue;
        targetValue.set("Foo");
        oneToOne.setTargetValue(targetValue);

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
    const babelwires::ValueHolder& sourceValue = dataPtr->getSourceValue();
    const babelwires::ValueHolder& targetValue = dataPtr->getTargetValue();
    ASSERT_NE(sourceValue, nullptr);
    ASSERT_NE(targetValue, nullptr);

    const auto *const sourceValueFromData = sourceValue->as<babelwires::StringValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->get(), "test serialization");

    const auto *const targetValueFromData = targetValue->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "Foo");
}

TEST(MapEntryDataTest, allToOneSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<babelwires::StringType>();  

        babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, babelwires::StringType::getThisType());

        babelwires::StringValue targetValue;
        targetValue.set("test serialization");

        allToOne.setTargetValue(targetValue);

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
    const babelwires::ValueHolder& targetValue = dataPtr->getTargetValue();
    ASSERT_NE(targetValue, nullptr);

    const auto *const targetValueFromData = targetValue->as<babelwires::StringValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "test serialization");
}

TEST(MapEntryDataTest, allToSameSerialize) {
    std::string serializedContents;
    {
            babelwires::TypeSystem typeSystem;
        typeSystem.addEntry<babelwires::StringType>();  

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
    typeSystem.addEntry<babelwires::StringType>();  
    typeSystem.addEntry<testDomain::TestEnum>();

    babelwires::OneToOneMapEntryData oneToOne(typeSystem, babelwires::StringType::getThisType(),
                                            testDomain::TestEnum::getThisType());

    babelwires::StringValue sourceValue;
    sourceValue.set("test serialization");

    oneToOne.setSourceValue(sourceValue);

    babelwires::EnumValue targetValue;
    targetValue.set("Foo");
    oneToOne.setTargetValue(targetValue);

    auto dataPtr = oneToOne.clone();

    ASSERT_NE(dataPtr, nullptr);
    const babelwires::ValueHolder& sourceValueInClone = dataPtr->getSourceValue();
    const babelwires::ValueHolder& targetValueInClone = dataPtr->getTargetValue();
    ASSERT_NE(sourceValueInClone, nullptr);
    ASSERT_NE(targetValueInClone, nullptr);

    const auto *const sourceValueFromData = sourceValueInClone->as<babelwires::StringValue>();
    ASSERT_NE(sourceValueFromData, nullptr);
    EXPECT_EQ(sourceValueFromData->get(), "test serialization");

    const auto *const targetValueFromData = targetValueInClone->as<babelwires::EnumValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "Foo");
}

TEST(MapEntryDataTest, allToOneClone) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();  

    babelwires::AllToOneFallbackMapEntryData allToOne(typeSystem, babelwires::StringType::getThisType());

    babelwires::StringValue targetValue;
    targetValue.set("test serialization");

    allToOne.setTargetValue(targetValue);

    auto dataPtr = allToOne.clone();

    ASSERT_NE(dataPtr, nullptr);
    const babelwires::ValueHolder& targetValueInClone = dataPtr->getTargetValue();
    ASSERT_NE(targetValueInClone, nullptr);

    const auto *const targetValueFromData = targetValueInClone->as<babelwires::StringValue>();
    ASSERT_NE(targetValueFromData, nullptr);
    EXPECT_EQ(targetValueFromData->get(), "test serialization");
}

TEST(MapEntryDataTest, allToSameClone) {
    babelwires::TypeSystem typeSystem;
    typeSystem.addEntry<babelwires::StringType>();  

    babelwires::AllToSameFallbackMapEntryData allToSame;

    auto dataPtr = allToSame.clone();

    ASSERT_NE(dataPtr, nullptr);
}
