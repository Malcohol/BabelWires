#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordTypeTest, simpleRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestSimpleRecordType recordType;

    EXPECT_FALSE(recordType.isOptional(testUtils::TestSimpleRecordType::getInt0Id()));
    EXPECT_FALSE(recordType.isOptional(testUtils::TestSimpleRecordType::getInt1Id()));
    EXPECT_TRUE(recordType.getOptionalFieldIds().empty());

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRecordValue = newValue->as<babelwires::RecordValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestSimpleRecordType::getInt0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestSimpleRecordType::getInt1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    EXPECT_NE(newRecordValue->getValue(testUtils::TestSimpleRecordType::getInt0Id())->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testUtils::TestSimpleRecordType::getInt1Id())->as<babelwires::IntValue>(),
              nullptr);

    EXPECT_EQ(recordType.getNumActiveFields(newValue), 0);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newRecordValue));
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

    EXPECT_EQ(recordType.getNumChildren(newValue), 2);

    const auto [child0, step0, typeRef0] = recordType.getChild(newValue, 0);
    const auto [child1, step1, typeRef1] = recordType.getChild(newValue, 1);

    EXPECT_NE((*child0)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE((*child1)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE(step0.asField(), nullptr);
    EXPECT_NE(step1.asField(), nullptr);
    EXPECT_EQ(*step0.asField(), testUtils::TestSimpleRecordType::getInt0Id());
    EXPECT_EQ(*step1.asField(), testUtils::TestSimpleRecordType::getInt1Id());

    const babelwires::Type& type0 = typeRef0.resolve(testEnvironment.m_typeSystem);
    const babelwires::Type& type1 = typeRef1.resolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type0.as<babelwires::IntType>(), nullptr);
    EXPECT_NE(type1.as<babelwires::IntType>(), nullptr);

    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step0), 0);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step1), 1);
}

namespace {
    void verifyComplexRecord(const babelwires::TypeSystem& typeSystem,
                             const testUtils::TestComplexRecordType& recordType, const babelwires::ValueHolder& value,
                             bool isOpIntActive, bool isOpRecActive) {
        auto* const recordValue = value->as<babelwires::RecordValue>();
        EXPECT_NE(recordValue, nullptr);

        EXPECT_NE(recordValue->tryGetValue(testUtils::TestComplexRecordType::getInt0Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testUtils::TestComplexRecordType::getOpIntId()) != nullptr, isOpIntActive);
        EXPECT_NE(recordValue->tryGetValue(testUtils::TestComplexRecordType::getSubrecordId()), nullptr);
        EXPECT_NE(recordValue->tryGetValue(testUtils::TestComplexRecordType::getInt1Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testUtils::TestComplexRecordType::getOpRecId()) != nullptr, isOpRecActive);

        EXPECT_TRUE(testUtils::areEqualSets(
            recordType.getOptionalFieldIds(),
            {testUtils::TestComplexRecordType::getOpIntId(), testUtils::TestComplexRecordType::getOpRecId()}));

        EXPECT_NE(recordValue->getValue(testUtils::TestComplexRecordType::getInt0Id())->as<babelwires::IntValue>(),
                  nullptr);
        EXPECT_NE(
            recordValue->getValue(testUtils::TestComplexRecordType::getSubrecordId())->as<babelwires::RecordValue>(),
            nullptr);
        EXPECT_NE(recordValue->getValue(testUtils::TestComplexRecordType::getInt1Id())->as<babelwires::IntValue>(),
                  nullptr);

        int numOptionals = 0;
        if (isOpIntActive) {
            EXPECT_NE(recordValue->getValue(testUtils::TestComplexRecordType::getOpIntId())->as<babelwires::IntValue>(),
                      nullptr);
            ++numOptionals;
        }
        if (isOpRecActive) {
            EXPECT_NE(
                recordValue->getValue(testUtils::TestComplexRecordType::getOpRecId())->as<babelwires::RecordValue>(),
                nullptr);
            ++numOptionals;
        }

        EXPECT_EQ(recordType.getNumActiveFields(value), numOptionals);

        EXPECT_EQ(recordType.getNumChildren(value), 3 + numOptionals);

        std::vector<std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>>
            childInfos;
        std::vector<const babelwires::Type*> types;

        for (unsigned int i = 0; i < 3 + numOptionals; ++i) {
            childInfos.emplace_back(recordType.getChild(value, i));
            types.emplace_back(&std::get<2>(childInfos.back()).resolve(typeSystem));
        }

        unsigned int int0Index = 0;
        unsigned int opIntIndex = 1;
        unsigned int subrecordIndex = int0Index + 1 + (isOpIntActive ? 1 : 0);
        unsigned int int1Index = subrecordIndex + 1;
        unsigned int opRecIndex = int1Index + 1;

        EXPECT_EQ(*std::get<1>(childInfos[int0Index]).asField(), testUtils::TestComplexRecordType::getInt0Id());
        if (isOpIntActive) {
            EXPECT_EQ(*std::get<1>(childInfos[opIntIndex]).asField(), testUtils::TestComplexRecordType::getOpIntId());
        }
        EXPECT_EQ(*std::get<1>(childInfos[subrecordIndex]).asField(),
                  testUtils::TestComplexRecordType::getSubrecordId());
        EXPECT_EQ(*std::get<1>(childInfos[int1Index]).asField(), testUtils::TestComplexRecordType::getInt1Id());
        if (isOpRecActive) {
            EXPECT_EQ(*std::get<1>(childInfos[opRecIndex]).asField(), testUtils::TestComplexRecordType::getOpRecId());
        }

        EXPECT_NE(types[int0Index]->as<babelwires::IntType>(), nullptr);
        if (isOpIntActive) {
            EXPECT_NE(types[opIntIndex]->as<babelwires::IntType>(), nullptr);
        }
        EXPECT_NE(types[subrecordIndex]->as<babelwires::RecordType>(), nullptr);
        EXPECT_NE(types[int1Index]->as<babelwires::IntType>(), nullptr);
        if (isOpRecActive) {
            EXPECT_NE(types[opRecIndex]->as<babelwires::RecordType>(), nullptr);
        }

        for (unsigned int i = 0; i < 3 + numOptionals; ++i) {
            EXPECT_EQ(recordType.getChildIndexFromStep(value, std::get<1>(childInfos[i])), i);
            EXPECT_TRUE(types[i]->isValidValue(typeSystem, *std::get<0>(childInfos[i])->getUnsafe()));
        }
    }
} // namespace

TEST(RecordTypeTest, complexRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestComplexRecordType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);
}

TEST(RecordTypeTest, activateOptional) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestComplexRecordType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testUtils::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, false);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testUtils::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true);

    recordType.deactivateField(newValue, testUtils::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, true);

    recordType.deactivateField(newValue, testUtils::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);
}
