#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordWithVariantsTypeTest, tags) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagAId()));
    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagBId()));
    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagCId()));

    EXPECT_FALSE(recordType.isTag(testUtils::getTestRegisteredIdentifier("foo")));

    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagAId()), 0);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagBId()), 1);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagCId()), 2);

    EXPECT_TRUE(testUtils::areEqualSets(recordType.getTags(), {testUtils::TestRecordWithVariantsType::getTagAId(),
                                                               testUtils::TestRecordWithVariantsType::getTagBId(),
                                                               testUtils::TestRecordWithVariantsType::getTagCId(),
                                                               testUtils::TestRecordWithVariantsType::getTagDId()}));
}

TEST(RecordWithVariantsTypeTest, value) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    // The second tag of the test record is the default.
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagBId());

    const auto* newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagAId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagAId());

    newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);
    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagAId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagCId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagCId());

    newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);
    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagCId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);
}

TEST(RecordWithVariantsTypeTest, getFieldsRemovedByChangeOfBranch) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testUtils::TestRecordWithVariantsType::getTagAId()),
        {testUtils::TestRecordWithVariantsType::getFieldB0Id(),
         testUtils::TestRecordWithVariantsType::getFieldBCId()}));
    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testUtils::TestRecordWithVariantsType::getTagBId()), {}));
    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testUtils::TestRecordWithVariantsType::getTagCId()),
        {testUtils::TestRecordWithVariantsType::getFieldB0Id(),
         testUtils::TestRecordWithVariantsType::getFieldABId()}));
}

TEST(RecordWithVariantsTypeTest, isValidValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagAId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagCId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagBId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    babelwires::Value& nonConstValue = newValue.copyContentsAndGetNonConst();
    auto* nonConstRecordValue = nonConstValue.as<babelwires::RecordWithVariantsValue>();    
    EXPECT_NE(nonConstRecordValue, nullptr);

    // An extra field is allowed
    nonConstRecordValue->setValue(babelwires::ShortId("foo"), babelwires::IntValue());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Inconsistent tag is not
    nonConstRecordValue->setTag(testUtils::TestRecordWithVariantsType::getTagAId());
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Set back
    nonConstRecordValue->setTag(testUtils::TestRecordWithVariantsType::getTagBId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Removing a field is not.
    nonConstRecordValue->removeValue(testUtils::TestRecordWithVariantsType::getFieldABId());
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));
}

namespace {
    void verifyComplexRecord(const babelwires::TypeSystem& typeSystem,
                             const testUtils::TestRecordWithVariantsType& recordType, const babelwires::ValueHolder& value,
                             babelwires::ShortId expectedTag) {
        auto* const recordValue = value->as<babelwires::RecordWithVariantsValue>();
        EXPECT_NE(recordValue, nullptr);

        EXPECT_NE(recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
        EXPECT_NE(recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);

        const bool isA = (expectedTag == testUtils::TestRecordWithVariantsType::getTagAId());
        const bool isB = (expectedTag == testUtils::TestRecordWithVariantsType::getTagBId());
        const bool isC = (expectedTag == testUtils::TestRecordWithVariantsType::getTagCId());

        EXPECT_EQ((recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()) != nullptr), isA);
        EXPECT_EQ((recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()) != nullptr), isA);
        EXPECT_EQ((recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()) != nullptr), isA || isB);
        EXPECT_EQ((recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()) != nullptr), isB);
        EXPECT_EQ((recordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()) != nullptr), isB || isC);

        const unsigned int numChildren = 2 + (isC ? 1 : 3);

        EXPECT_EQ(recordType.getNumChildren(value), numChildren);

        std::vector<std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypeRef&>>
            childInfos;
        std::vector<const babelwires::Type*> types;

        for (unsigned int i = 0; i < numChildren; ++i) {
            childInfos.emplace_back(recordType.getChild(value, i));
            types.emplace_back(&std::get<2>(childInfos.back()).resolve(typeSystem));
        }

        const unsigned int fieldA0Index = 0;
        const unsigned int ff0Index = isA ? 1 : 0;
        const unsigned int fieldB0Index = 1;
        const unsigned int fieldABIndex = 2;
        const unsigned int fieldA1Index = 3;
        const unsigned int ff1Index = isA ? 4 : (isB ? 3 : 1);
        const unsigned int fieldBCIndex = isB ? 4 : 2;

        EXPECT_EQ(*std::get<1>(childInfos[ff0Index]).asField(), testUtils::TestRecordWithVariantsType::getFf0Id());
        EXPECT_EQ(*std::get<1>(childInfos[ff1Index]).asField(), testUtils::TestRecordWithVariantsType::getFf1Id());

        if (isA) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldA0Index]).asField(), testUtils::TestRecordWithVariantsType::getFieldA0Id());
            EXPECT_EQ(*std::get<1>(childInfos[fieldABIndex]).asField(), testUtils::TestRecordWithVariantsType::getFieldABId());
            EXPECT_EQ(*std::get<1>(childInfos[fieldA1Index]).asField(), testUtils::TestRecordWithVariantsType::getFieldA1Id());
        }

        if (isB) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldB0Index]).asField(), testUtils::TestRecordWithVariantsType::getFieldB0Id());
            EXPECT_EQ(*std::get<1>(childInfos[fieldABIndex]).asField(), testUtils::TestRecordWithVariantsType::getFieldABId());
            EXPECT_EQ(*std::get<1>(childInfos[fieldBCIndex]).asField(), testUtils::TestRecordWithVariantsType::getFieldBCId());
        }

        if (isC) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldBCIndex]).asField(), testUtils::TestRecordWithVariantsType::getFieldBCId());
        }

        for (unsigned int i = 0; i < numChildren; ++i) {
            EXPECT_EQ(recordType.getChildIndexFromStep(value, std::get<1>(childInfos[i])), i);
            EXPECT_TRUE(types[i]->isValidValue(typeSystem, *std::get<0>(childInfos[i])->getUnsafe()));
        }
    }
}

TEST(RecordWithVariantsTypeTest, traversal) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);   

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value, testUtils::TestRecordWithVariantsType::getTagBId());

    recordType.selectTag(testEnvironment.m_typeSystem, value, testUtils::TestRecordWithVariantsType::getTagAId());
    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value, testUtils::TestRecordWithVariantsType::getTagAId());

    recordType.selectTag(testEnvironment.m_typeSystem, value, testUtils::TestRecordWithVariantsType::getTagCId());
    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value, testUtils::TestRecordWithVariantsType::getTagCId());
}

TEST(RecordWithVariantsTypeTest, getChildNonConstOfFixedField) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);   

    const unsigned int ff1Index = 3;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, ff1Index)).asField(), testUtils::TestRecordWithVariantsType::getFf1Id());    

    auto [value0, step0, type0] = recordType.getChild(value, ff1Index);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, ff1Index);

    EXPECT_EQ(*valueHolder0, *value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0, type1);

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, ff1Index);
    EXPECT_EQ(*value2, valueHolder1);   
}


TEST(RecordWithVariantsTypeTest, getChildNonConstOfFieldInBranch) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);   

    recordType.selectTag(testEnvironment.m_typeSystem, value, testUtils::TestRecordWithVariantsType::getTagCId());

    const unsigned int fieldBCIndex = 2;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, fieldBCIndex)).asField(), testUtils::TestRecordWithVariantsType::getFieldBCId());    

    auto [value0, step0, type0] = recordType.getChild(value, fieldBCIndex);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, fieldBCIndex);

    EXPECT_EQ(*valueHolder0, *value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0, type1);

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, fieldBCIndex);
    EXPECT_EQ(*value2, valueHolder1);   
}

TEST(RecordWithVariantsTypeTest, subtype) 
{
    testUtils::TestEnvironment testEnvironment;

    struct RecordWithNoFields : babelwires::RecordWithVariantsType {
        RecordWithNoFields() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {}) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("NoFields"), 1);
    };

    struct RecordA0 : babelwires::RecordWithVariantsType {
        RecordA0() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA", 1), babelwires::DefaultIntType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordA0"), 1);
    };

    struct RecordA1 : babelwires::RecordWithVariantsType {
        RecordA1() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA", 2), babelwires::DefaultIntType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordA1"), 1);
    };

    struct RecordB : babelwires::RecordWithVariantsType {
        RecordB() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordB"), 1);
    };

    struct RecordAB : babelwires::RecordWithVariantsType {
        RecordAB() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAB"), 1);
    };

    struct RecordAS : babelwires::RecordWithVariantsType {
        RecordAS() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA", 3), babelwires::StringType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAS"), 1);
    };

    struct RecordAV0 : babelwires::RecordWithVariantsType {
        RecordAV0() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1), testUtils::getTestRegisteredIdentifier("tag0", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("field0"), babelwires::DefaultIntType::getThisIdentifier(), {testUtils::getTestRegisteredIdentifier("tag0", 1)}}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAV0"), 1);
    };

    struct RecordABV0 : babelwires::RecordWithVariantsType {
        RecordABV0() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1), testUtils::getTestRegisteredIdentifier("tag0", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("field0"), babelwires::DefaultIntType::getThisIdentifier(), {testUtils::getTestRegisteredIdentifier("tag0", 1)}},
            {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisIdentifier()}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordABV0"), 1);
    };

    struct RecordABV01 : babelwires::RecordWithVariantsType {
        RecordABV01() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1), testUtils::getTestRegisteredIdentifier("tag0", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("field0"), babelwires::DefaultIntType::getThisIdentifier(), {testUtils::getTestRegisteredIdentifier("tag0", 1)}},
            {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("field1"), babelwires::DefaultIntType::getThisIdentifier(), {testUtils::getTestRegisteredIdentifier("tag0", 1)}}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordABV01"), 1);
    };

    struct RecordAVB : babelwires::RecordWithVariantsType {
        RecordAVB() : RecordWithVariantsType({testUtils::getTestRegisteredIdentifier("dtag", 1), testUtils::getTestRegisteredIdentifier("tag0", 1)}, {
            {testUtils::getTestRegisteredIdentifier("fieldA"), babelwires::DefaultIntType::getThisIdentifier()},
            {testUtils::getTestRegisteredIdentifier("fieldB"), babelwires::DefaultIntType::getThisIdentifier(), {testUtils::getTestRegisteredIdentifier("dtag", 1)}}
        }) {}
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("RecordAVB"), 1);
    };

    testEnvironment.m_typeSystem.addEntry<RecordWithNoFields>();
    testEnvironment.m_typeSystem.addEntry<RecordA0>();
    testEnvironment.m_typeSystem.addEntry<RecordA1>();
    testEnvironment.m_typeSystem.addEntry<RecordAS>();
    testEnvironment.m_typeSystem.addEntry<RecordB>();
    testEnvironment.m_typeSystem.addEntry<RecordAB>();
    testEnvironment.m_typeSystem.addEntry<RecordAV0>();
    testEnvironment.m_typeSystem.addEntry<RecordABV0>();
    testEnvironment.m_typeSystem.addEntry<RecordABV01>();
    testEnvironment.m_typeSystem.addEntry<RecordAVB>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordWithNoFields::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordWithNoFields::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordA1::getThisIdentifier()), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA1::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordAB::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordB::getThisIdentifier(), RecordAB::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);

    // Incompatible types
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAS::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordAS::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAS::getThisIdentifier(), RecordAB::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);

    // With variants
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordAV0::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAV0::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordABV0::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordABV0::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAB::getThisIdentifier(), RecordABV0::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordABV0::getThisIdentifier(), RecordAB::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordABV0::getThisIdentifier(), RecordABV01::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordABV01::getThisIdentifier(), RecordABV0::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordA0::getThisIdentifier(), RecordAVB::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAVB::getThisIdentifier(), RecordA0::getThisIdentifier()), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAB::getThisIdentifier(), RecordAVB::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(RecordAVB::getThisIdentifier(), RecordAB::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(RecordWithVariantsTypeTest, featureChanges)
{
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature valueFeature(testEnvironment.m_typeSystem, testUtils::TestRecordWithVariantsType::getThisIdentifier());
    valueFeature.setToDefault();

    const testUtils::TestRecordWithVariantsType* recordWithVariantsType = valueFeature.getType().as<testUtils::TestRecordWithVariantsType>();
    ASSERT_NE(recordWithVariantsType, nullptr);

    valueFeature.clearChanges();
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    {
        babelwires::BackupScope scope(valueFeature);
        babelwires::ValueHolder value = valueFeature.getValue();
        recordWithVariantsType->selectTag(testEnvironment.m_typeSystem, value, testUtils::TestRecordWithVariantsType::getTagAId());
        valueFeature.setValue(value);
    }
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::BackupScope scope(valueFeature);
        babelwires::ValueHolder value = valueFeature.getValue();
        recordWithVariantsType->selectTag(testEnvironment.m_typeSystem, value, testUtils::TestRecordWithVariantsType::getTagDId());
        valueFeature.setValue(value);
    }
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::BackupScope scope(valueFeature);
        babelwires::Path pathToInt;
        pathToInt.pushStep(babelwires::PathStep(testUtils::TestRecordWithVariantsType::getFf1Id()));
        pathToInt.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));
        babelwires::ValueHolder& value = valueFeature.setModifiable(pathToInt);
        value = babelwires::IntValue(15);
    }
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
}

TEST(RecordWithVariantsTypeTest, valueEquality)
{
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder value0 = recordType.createValue(testEnvironment.m_typeSystem);
    babelwires::ValueHolder value1 = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value0);
    EXPECT_EQ(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value1, testUtils::TestRecordWithVariantsType::getTagAId());

    EXPECT_NE(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value0, testUtils::TestRecordWithVariantsType::getTagDId());

    EXPECT_NE(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value0, testUtils::TestRecordWithVariantsType::getTagAId());

    EXPECT_EQ(value0, value1);
}

TEST(RecordWithVariantsTypeTest, valueHash)
{
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const std::size_t hash0 = newValue->getHash();

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagAId());

    const std::size_t hash1 = newValue->getHash();

    EXPECT_NE(hash0, hash1);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagDId());

    const std::size_t hash2 = newValue->getHash();

    EXPECT_NE(hash0, hash2);
    EXPECT_NE(hash1, hash2);
}

