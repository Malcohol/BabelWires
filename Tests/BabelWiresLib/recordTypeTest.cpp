#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Domains/TestDomain/testEnum.hpp>
#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testRecordTypeHierarchy.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(RecordTypeTest, simpleRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestSimpleRecordType recordType(testEnvironment.m_typeSystem);

    EXPECT_FALSE(recordType.isOptional(testDomain::TestSimpleRecordType::getInt0Id()));
    EXPECT_FALSE(recordType.isOptional(testDomain::TestSimpleRecordType::getInt1Id()));
    EXPECT_TRUE(recordType.getOptionalFieldIds().empty());

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRecordValue = newValue->tryAs<babelwires::RecordValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestSimpleRecordType::getInt0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestSimpleRecordType::getInt1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    EXPECT_NE(newRecordValue->getValue(testDomain::TestSimpleRecordType::getInt0Id())->tryAs<babelwires::IntValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testDomain::TestSimpleRecordType::getInt1Id())->tryAs<babelwires::IntValue>(),
              nullptr);

    EXPECT_EQ(recordType.getNumActiveFields(newValue), 0);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newRecordValue));
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

    EXPECT_EQ(recordType.getNumChildren(newValue), 2);

    const auto [child0, step0, type0] = recordType.getChild(newValue, 0);
    const auto [child1, step1, type1] = recordType.getChild(newValue, 1);

    EXPECT_NE((*child0)->tryAs<babelwires::IntValue>(), nullptr);
    EXPECT_NE((*child1)->tryAs<babelwires::IntValue>(), nullptr);
    EXPECT_NE(step0.asField(), nullptr);
    EXPECT_NE(step1.asField(), nullptr);
    EXPECT_EQ(*step0.asField(), testDomain::TestSimpleRecordType::getInt0Id());
    EXPECT_EQ(*step1.asField(), testDomain::TestSimpleRecordType::getInt1Id());

    EXPECT_NE(type0->tryAs<babelwires::IntType>(), nullptr);
    EXPECT_NE(type1->tryAs<babelwires::IntType>(), nullptr);

    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step0), 0);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step1), 1);
}

namespace {
    void verifyComplexRecord(const babelwires::TypeSystem& typeSystem,
                             const testDomain::TestComplexRecordType& recordType, const babelwires::ValueHolder& value,
                             bool isOpIntActive, bool isOpRecActive, bool isOpIntOnActive) {
        auto* const recordValue = value->tryAs<babelwires::RecordValue>();
        EXPECT_NE(recordValue, nullptr);

        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getInt0Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testDomain::TestComplexRecordType::getOpIntId()) != nullptr, isOpIntActive);
        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getSubrecordId()), nullptr);
        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getInt1Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testDomain::TestComplexRecordType::getOpRecId()) != nullptr, isOpRecActive);

        EXPECT_EQ(recordType.isActivated(value, testDomain::TestComplexRecordType::getOpIntId()), isOpIntActive);
        EXPECT_EQ(recordType.isActivated(value, testDomain::TestComplexRecordType::getOpRecId()), isOpRecActive);

        EXPECT_TRUE(testUtils::areEqualSets(recordType.getOptionalFieldIds(),
                                            {testDomain::TestComplexRecordType::getOpIntId(),
                                             testDomain::TestComplexRecordType::getOpRecId(),
                                             testDomain::TestComplexRecordType::getOnOptId()}));

        EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getInt0Id())->tryAs<babelwires::IntValue>(),
                  nullptr);
        EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getSubrecordId())
                      ->tryAs<babelwires::RecordValue>(),
                  nullptr);
        EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getInt1Id())->tryAs<babelwires::IntValue>(),
                  nullptr);

        int numOptionals = 0;
        if (isOpIntActive) {
            EXPECT_NE(
                recordValue->getValue(testDomain::TestComplexRecordType::getOpIntId())->tryAs<babelwires::IntValue>(),
                nullptr);
            ++numOptionals;
        }
        if (isOpRecActive) {
            EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getOpRecId())
                          ->tryAs<babelwires::RecordValue>(),
                      nullptr);
            ++numOptionals;
        }
        if (isOpIntOnActive) {
            EXPECT_NE(
                recordValue->getValue(testDomain::TestComplexRecordType::getOnOptId())->tryAs<babelwires::IntValue>(),
                nullptr);
            ++numOptionals;
        }

        EXPECT_EQ(recordType.getNumActiveFields(value), numOptionals);

        EXPECT_EQ(recordType.getNumChildren(value),
                  testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals);

        std::vector<std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>>
            childInfos;
        std::vector<const babelwires::Type*> types;

        for (unsigned int i = 0; i < testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals; ++i) {
            childInfos.emplace_back(recordType.getChild(value, i));
            types.emplace_back(std::get<2>(childInfos.back()).get());
        }

        unsigned int int0Index = 0;
        unsigned int opIntIndex = 1;
        unsigned int subrecordIndex = int0Index + 1 + (isOpIntActive ? 1 : 0);
        unsigned int int1Index = subrecordIndex + 1;
        unsigned int opRecIndex = int1Index + 1;

        EXPECT_EQ(*std::get<1>(childInfos[int0Index]).asField(), testDomain::TestComplexRecordType::getInt0Id());
        if (isOpIntActive) {
            EXPECT_EQ(*std::get<1>(childInfos[opIntIndex]).asField(), testDomain::TestComplexRecordType::getOpIntId());
        }
        EXPECT_EQ(*std::get<1>(childInfos[subrecordIndex]).asField(),
                  testDomain::TestComplexRecordType::getSubrecordId());
        EXPECT_EQ(*std::get<1>(childInfos[int1Index]).asField(), testDomain::TestComplexRecordType::getInt1Id());
        if (isOpRecActive) {
            EXPECT_EQ(*std::get<1>(childInfos[opRecIndex]).asField(), testDomain::TestComplexRecordType::getOpRecId());
        }

        EXPECT_NE(types[int0Index]->tryAs<babelwires::IntType>(), nullptr);
        if (isOpIntActive) {
            EXPECT_NE(types[opIntIndex]->tryAs<babelwires::IntType>(), nullptr);
        }
        EXPECT_NE(types[subrecordIndex]->tryAs<babelwires::RecordType>(), nullptr);
        EXPECT_NE(types[int1Index]->tryAs<babelwires::IntType>(), nullptr);
        if (isOpRecActive) {
            EXPECT_NE(types[opRecIndex]->tryAs<babelwires::RecordType>(), nullptr);
        }

        for (unsigned int i = 0; i < testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals; ++i) {
            EXPECT_EQ(recordType.getChildIndexFromStep(value, std::get<1>(childInfos[i])), i);
            EXPECT_TRUE(types[i]->isValidValue(typeSystem, *std::get<0>(childInfos[i])->getUnsafe()));
        }
    }
} // namespace

TEST(RecordTypeTest, complexRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, true);
}

TEST(RecordTypeTest, activateOptional) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, true);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testDomain::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, false, true);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testDomain::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true, true);

    recordType.deactivateField(newValue, testDomain::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, true, true);

    recordType.deactivateField(newValue, testDomain::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, true);

    recordType.deactivateField(newValue, testDomain::TestComplexRecordType::getOnOptId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, false);
}

TEST(RecordTypeTest, selectOptionals) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, true);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue,
                               {{testDomain::TestComplexRecordType::getOpIntId(), true}});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, false, true);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue,
                               {{testDomain::TestComplexRecordType::getOpIntId(), true},
                                {testDomain::TestComplexRecordType::getOpRecId(), true},
                                {testDomain::TestComplexRecordType::getOnOptId(), false}});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true, false);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue,
                               {{testDomain::TestComplexRecordType::getOpRecId(), true}});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, true, true);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue, {});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, true);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue,
                               {{testDomain::TestComplexRecordType::getOpIntId(), true},
                                {testDomain::TestComplexRecordType::getOpRecId(), true},
                                {testDomain::TestComplexRecordType::getOnOptId(), true}});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true, true);

    recordType.selectOptionals(testEnvironment.m_typeSystem, newValue,
                               {{testDomain::TestComplexRecordType::getOpRecId(), false},
                                {testDomain::TestComplexRecordType::getOnOptId(), false}});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false, false);
}

TEST(RecordTypeTest, getChildNonConstFixedField) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpRecId());

    const unsigned int int0Index = 0;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, int0Index)).asField(),
              testDomain::TestComplexRecordType::getInt0Id());

    auto [value0, step0, type0] = recordType.getChild(value, int0Index);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, int0Index);

    EXPECT_EQ(*valueHolder0, **value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0->getTypeExp(), type1->getTypeExp());

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, int0Index);
    EXPECT_EQ(*value2, valueHolder1);
}

TEST(RecordTypeTest, getChildNonConstOptionalField) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpRecId());

    const unsigned int opRecIndex = 3;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, opRecIndex)).asField(),
              testDomain::TestComplexRecordType::getOpRecId());

    auto [value0, step0, type0] = recordType.getChild(value, opRecIndex);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, opRecIndex);

    EXPECT_EQ(*valueHolder0, **value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0->getTypeExp(), type1->getTypeExp());

    const auto& opRecType = type0->tryAs<babelwires::RecordType>();

    // Test modification by modifying a field within the field.

    auto [subvalue0, substep0, subtype0] = opRecType->getChildNonConst(*value1, 0);

    babelwires::ValueHolder valueHolder1 = *value1;

    *subvalue0 = babelwires::IntValue(15);

    auto [value2, step2, type2] = recordType.getChild(value, opRecIndex);
    EXPECT_EQ(*value2, valueHolder1);

    auto [subvalue2, substep2, subtype2] = opRecType->getChild(*value2, 0);
    EXPECT_EQ(*subvalue2, *subvalue0);
}

TEST(RecordTypeTest, subtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr recordWithNoFields =
        testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordWithNoFields>();
    const babelwires::TypePtr recordA0 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordA0>();
    const babelwires::TypePtr recordA1 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordA1>();
    const babelwires::TypePtr recordAB = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAB>();
    const babelwires::TypePtr recordB = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordB>();
    const babelwires::TypePtr recordAS = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAS>();
    const babelwires::TypePtr recordAOpt = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAOpt>();
    const babelwires::TypePtr recordABOpt = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordABOpt>();
    const babelwires::TypePtr recordAOptFixed =
        testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAOptFixed>();
    const babelwires::TypePtr recordAOptS = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAOptS>();
    const babelwires::TypePtr recordAsub0 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAsub0>();
    const babelwires::TypePtr recordAsubBsup =
        testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordAsubBsup>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordWithNoFields, *recordA0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordWithNoFields),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordA1),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA1, *recordA0),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordAB), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordB, *recordAB), babelwires::SubtypeOrder::IsSupertype);

    // { A = TestSubEnum::erm, B = TestSubEnum::erm } is a member of both A0 and B.
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordB),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordB, *recordA0),
              babelwires::SubtypeOrder::IsIntersecting);

    // Incompatible types
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAS, *recordA0), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordAS), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAS, *recordAB), babelwires::SubtypeOrder::IsDisjoint);

    // With optionals.
    // { A = TestSubEnum::erm, Opt = 100 } is a member of A0 but not of AOpt, where Opt is expected to be a TestSubEnum.
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordAOpt),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOpt, *recordA0), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordABOpt),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordABOpt, *recordA0),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOpt, *recordAOptFixed),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOptFixed, *recordAOpt),
              babelwires::SubtypeOrder::IsSubtype);

    // { a : TestSubEnum::erm, b : TestSubEnum::erm } is a member of both.
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOptS, *recordABOpt),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordABOpt, *recordAOptS),
              babelwires::SubtypeOrder::IsIntersecting);

    // Intersecting with optionals
    // { A = TestSubEnum::erm } is a member of both.
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOpt, *recordAOptS),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAOptS, *recordAOpt),
              babelwires::SubtypeOrder::IsIntersecting);

    // With field subtypes
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAsub0, *recordA0),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordA0, *recordAsub0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAB, *recordAsubBsup),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordAsubBsup, *recordAB),
              babelwires::SubtypeOrder::IsIntersecting);
}

// Test the use of the constructor which takes a parent type.
TEST(RecordTypeTest, subtypeConstructor) {
    testUtils::TestEnvironment testEnvironment;

    const auto& abOptChild = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordABOptChild>();

    const std::vector<babelwires::RecordType::Field> fields = abOptChild->getFields();
    EXPECT_EQ(fields.size(), 5);
    EXPECT_EQ(fields[0].m_identifier, "A");
    EXPECT_EQ(fields[1].m_identifier, "B");
    EXPECT_EQ(fields[2].m_identifier, "Opt");
    EXPECT_EQ(fields[3].m_identifier, "C");
    EXPECT_EQ(fields[4].m_identifier, "Opt2");

    const std::vector<babelwires::ShortId> optionals = abOptChild->getOptionalFieldIds();
    EXPECT_EQ(optionals.size(), 2);
    EXPECT_EQ(optionals[0], "Opt");
    EXPECT_EQ(optionals[1], "Opt2");

    const babelwires::TypePtr recordABOpt = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordABOpt>();
    const babelwires::TypePtr recordABOptChild =
        testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordABOptChild>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordABOpt, *recordABOptChild),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordABOptChild, *recordABOpt),
              babelwires::SubtypeOrder::IsSubtype);
}

TEST(RecordTypeTest, featureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_typeSystem,
                                           testDomain::TestComplexRecordType::getThisIdentifier());
    valueFeature.setToDefault();

    const testDomain::TestComplexRecordType* recordType =
        valueFeature.getType()->tryAs<testDomain::TestComplexRecordType>();
    ASSERT_NE(recordType, nullptr);

    valueFeature.clearChanges();
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    {
        babelwires::ValueHolder value = valueFeature.getValue();
        recordType->activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpRecId());
        valueFeature.setValue(value);
    }
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::Path pathToInt;
        pathToInt.pushStep(testDomain::TestComplexRecordType::getOpRecId());
        pathToInt.pushStep(testDomain::TestSimpleRecordType::getInt0Id());
        valueFeature.setDescendentValue(pathToInt, babelwires::IntValue(15));
    }
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::ValueHolder value = valueFeature.getValue();
        recordType->deactivateField(value, testDomain::TestComplexRecordType::getOpRecId());
        valueFeature.setValue(value);
    }
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
}

TEST(RecordTypeTest, valueEquality) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value0 = recordType.createValue(testEnvironment.m_typeSystem);
    babelwires::ValueHolder value1 = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value0);
    EXPECT_EQ(value0, value1);

    recordType.activateField(testEnvironment.m_typeSystem, value0, testDomain::TestComplexRecordType::getOpRecId());

    EXPECT_NE(value0, value1);

    recordType.deactivateField(value0, testDomain::TestComplexRecordType::getOpRecId());

    EXPECT_EQ(value0, value1);

    recordType.activateField(testEnvironment.m_typeSystem, value0, testDomain::TestComplexRecordType::getOpIntId());

    EXPECT_NE(value0, value1);

    recordType.activateField(testEnvironment.m_typeSystem, value1, testDomain::TestComplexRecordType::getOpIntId());

    EXPECT_EQ(value0, value1);
}

TEST(RecordTypeTest, valueHash) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    const std::size_t hash0 = value->getHash();

    recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpIntId());

    const std::size_t hash1 = value->getHash();

    EXPECT_NE(hash0, hash1);

    recordType.deactivateField(value, testDomain::TestComplexRecordType::getOpIntId());

    const std::size_t hash2 = value->getHash();

    EXPECT_EQ(hash0, hash2);

    // Prepare to modify a child.
    auto index = recordType.getChildIndexFromStep(value, testDomain::TestComplexRecordType::getInt0Id());
    auto child = recordType.getChildNonConst(value, index);

    // Still unmodified.
    const std::size_t hash3 = value->getHash();
    EXPECT_EQ(hash0, hash3);

    // Now modified
    std::get<0>(child)->operator=(babelwires::IntValue(43));
    const std::size_t hash4 = value->getHash();
    EXPECT_NE(hash0, hash4);
}

TEST(RecordTypeTest, exceptions) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    EXPECT_THROW(recordType.deactivateField(value, testDomain::TestComplexRecordType::getOpIntId()),
                 babelwires::ModelException);
    EXPECT_NO_THROW(
        recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpIntId()));
    EXPECT_THROW(
        recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpIntId()),
        babelwires::ModelException);
    EXPECT_THROW(recordType.activateField(testEnvironment.m_typeSystem, value, "foo"), babelwires::ModelException);
}

TEST(RecordTypeTest, constructorBasics) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp recordTypeExp(
        babelwires::RecordTypeConstructor::getThisIdentifier(),
        babelwires::TypeConstructorArguments{
            {babelwires::DefaultIntType::getThisIdentifier(), babelwires::StringType::getThisIdentifier()},
            {babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("int0")),
             babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("str0"))}});

    babelwires::TypePtr type = recordTypeExp.resolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type->tryAs<babelwires::RecordType>());
    const babelwires::RecordType& recordType = type->as<babelwires::RecordType>();
    EXPECT_EQ(recordType.getFields().size(), 2);
    EXPECT_EQ(recordType.getFields()[0].m_identifier, "int0");
    EXPECT_EQ(recordType.getFields()[0].m_type->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(recordType.getFields()[1].m_identifier, "str0");
    EXPECT_EQ(recordType.getFields()[1].m_type->getTypeExp(), babelwires::StringType::getThisIdentifier());
}

TEST(RecordTypeTest, constructorWithOptionals) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp recordTypeExp(
        babelwires::RecordTypeConstructor::getThisIdentifier(),
        babelwires::TypeConstructorArguments{
            {babelwires::DefaultIntType::getThisIdentifier(), babelwires::StringType::getThisIdentifier()},
            {babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("int0")),
             babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("str0"),
                                      babelwires::RecordType::Optionality::optionalDefaultInactive)}});

    babelwires::TypePtr type = recordTypeExp.resolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type->tryAs<babelwires::RecordType>());
    const babelwires::RecordType& recordType = type->as<babelwires::RecordType>();
    EXPECT_EQ(recordType.getFields().size(), 2);
    EXPECT_EQ(recordType.getOptionalFieldIds().size(), 1);
    EXPECT_EQ(recordType.getFields()[0].m_identifier, "int0");
    EXPECT_EQ(recordType.getFields()[0].m_type->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(recordType.getFields()[0].m_optionality, babelwires::RecordType::Optionality::alwaysActive);
    EXPECT_EQ(recordType.getFields()[1].m_identifier, "str0");
    EXPECT_EQ(recordType.getFields()[1].m_type->getTypeExp(), babelwires::StringType::getThisIdentifier());
    EXPECT_EQ(recordType.getFields()[1].m_optionality, babelwires::RecordType::Optionality::optionalDefaultInactive);
}

TEST(RecordTypeTest, constructorBadArgs) {
    testUtils::TestEnvironment testEnvironment;

    {
        babelwires::TypeExp recordTypeExp(
            babelwires::RecordTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{{babelwires::DefaultIntType::getThisIdentifier()}, {/* No value */}});
        EXPECT_THROW(recordTypeExp.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeExp recordTypeExp(
            babelwires::RecordTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::DefaultIntType::getThisIdentifier(), babelwires::StringType::getThisIdentifier()},
                {babelwires::FieldIdValue("int0"), babelwires::IntValue(42)}});
        EXPECT_THROW(recordTypeExp.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
}

TEST(RecordTypeTest, constructorMakeRef) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp recordTypeExp = babelwires::RecordTypeConstructor::makeTypeExp(
        testUtils::getTestRegisteredIdentifier("int0"), babelwires::DefaultIntType::getThisIdentifier(),
        testUtils::getTestRegisteredIdentifier("str0"), babelwires::StringType::getThisIdentifier());

    babelwires::TypePtr type = recordTypeExp.resolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type->tryAs<babelwires::RecordType>());
    const babelwires::RecordType& recordType = type->as<babelwires::RecordType>();
    EXPECT_EQ(recordType.getFields().size(), 2);
    EXPECT_EQ(recordType.getFields()[0].m_identifier, "int0");
    EXPECT_EQ(recordType.getFields()[0].m_type->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(recordType.getFields()[1].m_identifier, "str0");
    EXPECT_EQ(recordType.getFields()[1].m_type->getTypeExp(), babelwires::StringType::getThisIdentifier());
}

TEST(RecordTypeTest, constructorName) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp recordTypeExp = babelwires::RecordTypeConstructor::makeTypeExp(
        "a", babelwires::StringType::getThisIdentifier(), "b", babelwires::DefaultIntType::getThisIdentifier(), "c",
        babelwires::StringType::getThisIdentifier());

    EXPECT_EQ(recordTypeExp.toString(), "Record{a, b, c : String, Integer, String}");
}

TEST(RecordTypeTest, typeExpSerialization) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp recordTypeExp(
    babelwires::RecordTypeConstructor::getThisIdentifier(),
    babelwires::TypeConstructorArguments{
        {babelwires::DefaultIntType::getThisIdentifier(), babelwires::StringType::getThisIdentifier(),
            babelwires::DefaultIntType::getThisIdentifier()},
        {babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("int0")),
            babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("str0"),
                                    babelwires::RecordType::Optionality::optionalDefaultInactive),
            babelwires::FieldIdValue(testUtils::getTestRegisteredIdentifier("int1"),
                                    babelwires::RecordType::Optionality::optionalDefaultActive)}});

    std::string serializedContents;
    {

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(recordTypeExp);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(deserializationReg, testEnvironment.m_log);
    ASSERT_TRUE(deserializer.parse(serializedContents));
    auto deserializedTypeExpResult = deserializer.deserializeObject<babelwires::TypeExp>();
    ASSERT_TRUE(deserializedTypeExpResult);
    auto deserializedTypeExp = std::move(*deserializedTypeExpResult);
    deserializer.finalize();

    EXPECT_EQ(recordTypeExp, *deserializedTypeExp);
}