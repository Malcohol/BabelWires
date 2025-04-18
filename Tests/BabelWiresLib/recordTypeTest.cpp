#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Domains/TestDomain/testEnum.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordTypeTest, simpleRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestSimpleRecordType recordType;

    EXPECT_FALSE(recordType.isOptional(testDomain::TestSimpleRecordType::getInt0Id()));
    EXPECT_FALSE(recordType.isOptional(testDomain::TestSimpleRecordType::getInt1Id()));
    EXPECT_TRUE(recordType.getOptionalFieldIds().empty());

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRecordValue = newValue->as<babelwires::RecordValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestSimpleRecordType::getInt0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestSimpleRecordType::getInt1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    EXPECT_NE(newRecordValue->getValue(testDomain::TestSimpleRecordType::getInt0Id())->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testDomain::TestSimpleRecordType::getInt1Id())->as<babelwires::IntValue>(),
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
    EXPECT_EQ(*step0.asField(), testDomain::TestSimpleRecordType::getInt0Id());
    EXPECT_EQ(*step1.asField(), testDomain::TestSimpleRecordType::getInt1Id());

    const babelwires::Type& type0 = typeRef0.resolve(testEnvironment.m_typeSystem);
    const babelwires::Type& type1 = typeRef1.resolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type0.as<babelwires::IntType>(), nullptr);
    EXPECT_NE(type1.as<babelwires::IntType>(), nullptr);

    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step0), 0);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step1), 1);
}

namespace {
    void verifyComplexRecord(const babelwires::TypeSystem& typeSystem,
                             const testDomain::TestComplexRecordType& recordType, const babelwires::ValueHolder& value,
                             bool isOpIntActive, bool isOpRecActive) {
        auto* const recordValue = value->as<babelwires::RecordValue>();
        EXPECT_NE(recordValue, nullptr);

        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getInt0Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testDomain::TestComplexRecordType::getOpIntId()) != nullptr, isOpIntActive);
        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getSubrecordId()), nullptr);
        EXPECT_NE(recordValue->tryGetValue(testDomain::TestComplexRecordType::getInt1Id()), nullptr);
        EXPECT_EQ(recordValue->tryGetValue(testDomain::TestComplexRecordType::getOpRecId()) != nullptr, isOpRecActive);

        EXPECT_EQ(recordType.isActivated(value, testDomain::TestComplexRecordType::getOpIntId()), isOpIntActive);
        EXPECT_EQ(recordType.isActivated(value, testDomain::TestComplexRecordType::getOpRecId()), isOpRecActive);

        EXPECT_TRUE(testUtils::areEqualSets(
            recordType.getOptionalFieldIds(),
            {testDomain::TestComplexRecordType::getOpIntId(), testDomain::TestComplexRecordType::getOpRecId()}));

        EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getInt0Id())->as<babelwires::IntValue>(),
                  nullptr);
        EXPECT_NE(
            recordValue->getValue(testDomain::TestComplexRecordType::getSubrecordId())->as<babelwires::RecordValue>(),
            nullptr);
        EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getInt1Id())->as<babelwires::IntValue>(),
                  nullptr);

        int numOptionals = 0;
        if (isOpIntActive) {
            EXPECT_NE(recordValue->getValue(testDomain::TestComplexRecordType::getOpIntId())->as<babelwires::IntValue>(),
                      nullptr);
            ++numOptionals;
        }
        if (isOpRecActive) {
            EXPECT_NE(
                recordValue->getValue(testDomain::TestComplexRecordType::getOpRecId())->as<babelwires::RecordValue>(),
                nullptr);
            ++numOptionals;
        }

        EXPECT_EQ(recordType.getNumActiveFields(value), numOptionals);

        EXPECT_EQ(recordType.getNumChildren(value),
                  testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals);

        std::vector<std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>>
            childInfos;
        std::vector<const babelwires::Type*> types;

        for (unsigned int i = 0; i < testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals; ++i) {
            childInfos.emplace_back(recordType.getChild(value, i));
            types.emplace_back(&std::get<2>(childInfos.back()).resolve(typeSystem));
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

        EXPECT_NE(types[int0Index]->as<babelwires::IntType>(), nullptr);
        if (isOpIntActive) {
            EXPECT_NE(types[opIntIndex]->as<babelwires::IntType>(), nullptr);
        }
        EXPECT_NE(types[subrecordIndex]->as<babelwires::RecordType>(), nullptr);
        EXPECT_NE(types[int1Index]->as<babelwires::IntType>(), nullptr);
        if (isOpRecActive) {
            EXPECT_NE(types[opRecIndex]->as<babelwires::RecordType>(), nullptr);
        }

        for (unsigned int i = 0; i < testDomain::TestComplexRecordType::s_numNonOptionalFields + numOptionals; ++i) {
            EXPECT_EQ(recordType.getChildIndexFromStep(value, std::get<1>(childInfos[i])), i);
            EXPECT_TRUE(types[i]->isValidValue(typeSystem, *std::get<0>(childInfos[i])->getUnsafe()));
        }
    }
} // namespace

TEST(RecordTypeTest, complexRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);
}

TEST(RecordTypeTest, activateOptional) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testDomain::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, false);

    recordType.activateField(testEnvironment.m_typeSystem, newValue, testDomain::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true);

    recordType.deactivateField(newValue, testDomain::TestComplexRecordType::getOpIntId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, true);

    recordType.deactivateField(newValue, testDomain::TestComplexRecordType::getOpRecId());

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);
}

TEST(RecordTypeTest, ensureActivated) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);

    recordType.ensureActivated(testEnvironment.m_typeSystem, newValue,
                               {testDomain::TestComplexRecordType::getOpIntId()});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, false);

    recordType.ensureActivated(
        testEnvironment.m_typeSystem, newValue,
        {testDomain::TestComplexRecordType::getOpIntId(), testDomain::TestComplexRecordType::getOpRecId()});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true);

    recordType.ensureActivated(testEnvironment.m_typeSystem, newValue,
                               {testDomain::TestComplexRecordType::getOpRecId()});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, true);

    recordType.ensureActivated(testEnvironment.m_typeSystem, newValue, {});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);

    recordType.ensureActivated(
        testEnvironment.m_typeSystem, newValue,
        {testDomain::TestComplexRecordType::getOpIntId(), testDomain::TestComplexRecordType::getOpRecId()});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, true, true);

    recordType.ensureActivated(testEnvironment.m_typeSystem, newValue, {});

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, newValue, false, false);
}

TEST(RecordTypeTest, getChildNonConstFixedField) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType;

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpRecId());

    const unsigned int int0Index = 0;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, int0Index)).asField(),
              testDomain::TestComplexRecordType::getInt0Id());

    auto [value0, step0, type0] = recordType.getChild(value, int0Index);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, int0Index);

    EXPECT_EQ(*valueHolder0, *value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0, type1);

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, int0Index);
    EXPECT_EQ(*value2, valueHolder1);
}

TEST(RecordTypeTest, getChildNonConstOptionalField) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestComplexRecordType recordType;

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    recordType.activateField(testEnvironment.m_typeSystem, value, testDomain::TestComplexRecordType::getOpRecId());

    const unsigned int opRecIndex = 3;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, opRecIndex)).asField(),
              testDomain::TestComplexRecordType::getOpRecId());

    auto [value0, step0, type0] = recordType.getChild(value, opRecIndex);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, opRecIndex);

    EXPECT_EQ(*valueHolder0, *value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0, type1);

    const babelwires::RecordType& opRecType = type0.resolve(testEnvironment.m_typeSystem).is<babelwires::RecordType>();

    // Test modification by modifying a field within the field.

    auto [subvalue0, substep0, subtype0] = opRecType.getChildNonConst(*value1, 0);

    babelwires::ValueHolder valueHolder1 = *value1;

    *subvalue0 = babelwires::IntValue(15);

    auto [value2, step2, type2] = recordType.getChild(value, opRecIndex);
    EXPECT_EQ(*value2, valueHolder1);

    auto [subvalue2, substep2, subtype2] = opRecType.getChild(*value2, 0);
    EXPECT_EQ(*subvalue2, *subvalue0);
}

TEST(RecordTypeTest, subtype) {
    testUtils::TestEnvironment testEnvironment;

    struct RecordWithNoFields : babelwires::RecordType {
        RecordWithNoFields()
            : RecordType({}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("NoFields"), 1);
    };

    struct RecordA0 : babelwires::RecordType {
        RecordA0()
            : RecordType(
                  {{testUtils::getTestRegisteredIdentifier("fieldA", 1), babelwires::DefaultIntType::getThisType()}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordA0"), 1);
    };

    struct RecordA1 : babelwires::RecordType {
        RecordA1()
            : RecordType(
                  {{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordA1"), 1);
    };

    struct RecordB : babelwires::RecordType {
        RecordB()
            : RecordType(
                  {{testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisType()}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordB"), 1);
    };

    struct RecordAB : babelwires::RecordType {
        RecordAB()
            : RecordType(
                  {{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()},
                   {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisType()}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAB"), 1);
    };

    struct RecordAS : babelwires::RecordType {
        RecordAS()
            : RecordType(
                  {{testUtils::getTestRegisteredIdentifier("fieldA", 3), babelwires::StringType::getThisType()}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAS"), 1);
    };

    struct RecordAOpt : babelwires::RecordType {
        RecordAOpt()
            : RecordType({{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fOpt"), babelwires::DefaultIntType::getThisType(),
                           babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAOpt"), 1);
    };

    struct RecordABOpt : babelwires::RecordType {
        RecordABOpt()
            : RecordType({{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fOpt"), babelwires::DefaultIntType::getThisType(),
                           babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordABOpt"), 1);
    };

    struct RecordAOptS : babelwires::RecordType {
        RecordAOptS()
            : RecordType({{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fOpt"), babelwires::StringType::getThisType(),
                           babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAOptS"), 1);
    };

    testEnvironment.m_typeSystem.addEntry<RecordWithNoFields>();
    testEnvironment.m_typeSystem.addEntry<RecordA0>();
    testEnvironment.m_typeSystem.addEntry<RecordA1>();
    testEnvironment.m_typeSystem.addEntry<RecordAS>();
    testEnvironment.m_typeSystem.addEntry<RecordB>();
    testEnvironment.m_typeSystem.addEntry<RecordAB>();
    testEnvironment.m_typeSystem.addEntry<RecordAOpt>();
    testEnvironment.m_typeSystem.addEntry<RecordABOpt>();
    testEnvironment.m_typeSystem.addEntry<RecordAOptS>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordWithNoFields::getThisType(), RecordA0::getThisType()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordWithNoFields::getThisType()),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordA1::getThisType()),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA1::getThisType(), RecordA0::getThisType()),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordAB::getThisType()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordB::getThisType(), RecordAB::getThisType()),
              babelwires::SubtypeOrder::IsSupertype);

    // Incompatible types
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAS::getThisType(), RecordA0::getThisType()),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordAS::getThisType()),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAS::getThisType(), RecordAB::getThisType()),
              babelwires::SubtypeOrder::IsUnrelated);

    // With optionals: Optional fields do not impact subtyping since they are not part of the type's contract,
    // unless the types are incompatible (see next sequence of tests, below).
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordAOpt::getThisType()),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAOpt::getThisType(), RecordA0::getThisType()),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisType(), RecordABOpt::getThisType()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordABOpt::getThisType(), RecordA0::getThisType()),
              babelwires::SubtypeOrder::IsSubtype);

    // Incompatible and optional
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAOpt::getThisType(), RecordAOptS::getThisType()),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAOptS::getThisType(), RecordAOpt::getThisType()),
              babelwires::SubtypeOrder::IsUnrelated);
}

// Test the use of the constructor which takes a parent type.
TEST(RecordTypeTest, subtypeConstructor) {
    testUtils::TestEnvironment testEnvironment;

    struct RecordParent : babelwires::RecordType {
        RecordParent()
            : RecordType({{testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fOpt"), babelwires::DefaultIntType::getThisType(),
                           babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordParent"), 1);
    };

    struct RecordChild : babelwires::RecordType {
        RecordChild(const babelwires::TypeSystem& typeSystem)
            : RecordType(RecordParent::getThisType().resolve(typeSystem).is<babelwires::RecordType>(),
                         {{testUtils::getTestRegisteredIdentifier("fieldC"), babelwires::DefaultIntType::getThisType()},
                          {testUtils::getTestRegisteredIdentifier("fOpt2"), babelwires::DefaultIntType::getThisType(),
                           babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordChild"), 1);
    };
    testEnvironment.m_typeSystem.addEntry<RecordParent>();
    const babelwires::RecordType& foo =
        RecordParent::getThisType().resolve(testEnvironment.m_typeSystem).is<babelwires::RecordType>();

    const babelwires::RecordType* const abOpt =
        testEnvironment.m_typeSystem.addEntry<RecordChild>(testEnvironment.m_typeSystem);

    const std::vector<babelwires::RecordType::Field> fields = abOpt->getFields();
    EXPECT_EQ(fields.size(), 5);
    EXPECT_EQ(fields[0].m_identifier, "fieldA");
    EXPECT_EQ(fields[1].m_identifier, "fieldB");
    EXPECT_EQ(fields[2].m_identifier, "fOpt");
    EXPECT_EQ(fields[3].m_identifier, "fieldC");
    EXPECT_EQ(fields[4].m_identifier, "fOpt2");

    const std::vector<babelwires::ShortId> optionals = abOpt->getOptionalFieldIds();
    EXPECT_EQ(optionals.size(), 2);
    EXPECT_EQ(optionals[0], "fOpt");
    EXPECT_EQ(optionals[1], "fOpt2");

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordParent::getThisType(), RecordChild::getThisType()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordChild::getThisType(), RecordParent::getThisType()),
              babelwires::SubtypeOrder::IsSubtype);
}

TEST(RecordTypeTest, featureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_typeSystem,
                                           testDomain::TestComplexRecordType::getThisType());
    valueFeature.setToDefault();

    const testDomain::TestComplexRecordType* recordType = valueFeature.getType().as<testDomain::TestComplexRecordType>();
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
    testDomain::TestComplexRecordType recordType;

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
    testDomain::TestComplexRecordType recordType;

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
    testDomain::TestComplexRecordType recordType;

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