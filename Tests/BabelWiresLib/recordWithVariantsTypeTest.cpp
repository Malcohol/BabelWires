#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Domains/TestDomain/testEnum.hpp>
#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testRecordWithVariantsType.hpp>
#include <Domains/TestDomain/testRecordWithVariantsTypeHierarchy.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordWithVariantsTypeTest, tags) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    EXPECT_TRUE(recordType.isTag(testDomain::TestRecordWithVariantsType::getTagAId()));
    EXPECT_TRUE(recordType.isTag(testDomain::TestRecordWithVariantsType::getTagBId()));
    EXPECT_TRUE(recordType.isTag(testDomain::TestRecordWithVariantsType::getTagCId()));

    EXPECT_FALSE(recordType.isTag("foo"));

    EXPECT_EQ(recordType.getIndexOfTag(testDomain::TestRecordWithVariantsType::getTagAId()), 0);
    EXPECT_EQ(recordType.getIndexOfTag(testDomain::TestRecordWithVariantsType::getTagBId()), 1);
    EXPECT_EQ(recordType.getIndexOfTag(testDomain::TestRecordWithVariantsType::getTagCId()), 2);

    EXPECT_TRUE(testUtils::areEqualSets(recordType.getTags(), {testDomain::TestRecordWithVariantsType::getTagAId(),
                                                               testDomain::TestRecordWithVariantsType::getTagBId(),
                                                               testDomain::TestRecordWithVariantsType::getTagCId(),
                                                               testDomain::TestRecordWithVariantsType::getTagDId()}));
}

TEST(RecordWithVariantsTypeTest, value) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    // The second tag of the test record is the default.
    EXPECT_EQ(recordType.getSelectedTag(newValue), testDomain::TestRecordWithVariantsType::getTagBId());

    const auto* newRecordValue = newValue->tryAs<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_EQ(newRecordValue->getTag(), testDomain::TestRecordWithVariantsType::getTagBId());

    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagAId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testDomain::TestRecordWithVariantsType::getTagAId());

    newRecordValue = newValue->tryAs<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);
    EXPECT_EQ(newRecordValue->getTag(), testDomain::TestRecordWithVariantsType::getTagAId());

    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagCId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testDomain::TestRecordWithVariantsType::getTagCId());

    newRecordValue = newValue->tryAs<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);
    EXPECT_EQ(newRecordValue->getTag(), testDomain::TestRecordWithVariantsType::getTagCId());

    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);
}

TEST(RecordWithVariantsTypeTest, getFieldsRemovedByChangeOfBranch) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    EXPECT_EQ(recordType.getSelectedTag(newValue), testDomain::TestRecordWithVariantsType::getTagBId());

    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testDomain::TestRecordWithVariantsType::getTagAId()),
        {testDomain::TestRecordWithVariantsType::getFieldB0Id(),
         testDomain::TestRecordWithVariantsType::getFieldBCId()}));
    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testDomain::TestRecordWithVariantsType::getTagBId()),
        {}));
    EXPECT_TRUE(testUtils::areEqualSets(
        recordType.getFieldsRemovedByChangeOfBranch(newValue, testDomain::TestRecordWithVariantsType::getTagCId()),
        {testDomain::TestRecordWithVariantsType::getFieldB0Id(),
         testDomain::TestRecordWithVariantsType::getFieldABId()}));
}

TEST(RecordWithVariantsTypeTest, isValidValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagAId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagCId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagBId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    babelwires::Value& nonConstValue = newValue.copyContentsAndGetNonConst();
    auto* nonConstRecordValue = nonConstValue.tryAs<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(nonConstRecordValue, nullptr);

    // An extra field is allowed
    nonConstRecordValue->setValue(babelwires::ShortId("foo"), babelwires::IntValue());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Inconsistent tag is not
    nonConstRecordValue->setTag(testDomain::TestRecordWithVariantsType::getTagAId());
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Set back
    nonConstRecordValue->setTag(testDomain::TestRecordWithVariantsType::getTagBId());
    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    // Removing a field is not.
    nonConstRecordValue->removeValue(testDomain::TestRecordWithVariantsType::getFieldABId());
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, *newValue));
}

namespace {
    void verifyComplexRecord(const babelwires::TypeSystem& typeSystem,
                             const testDomain::TestRecordWithVariantsType& recordType,
                             const babelwires::ValueHolder& value, babelwires::ShortId expectedTag) {
        auto* const recordValue = value->tryAs<babelwires::RecordWithVariantsValue>();
        EXPECT_NE(recordValue, nullptr);

        EXPECT_NE(recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf0Id()), nullptr);
        EXPECT_NE(recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFf1Id()), nullptr);

        const bool isA = (expectedTag == testDomain::TestRecordWithVariantsType::getTagAId());
        const bool isB = (expectedTag == testDomain::TestRecordWithVariantsType::getTagBId());
        const bool isC = (expectedTag == testDomain::TestRecordWithVariantsType::getTagCId());

        EXPECT_EQ((recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA0Id()) != nullptr), isA);
        EXPECT_EQ((recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldA1Id()) != nullptr), isA);
        EXPECT_EQ((recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldABId()) != nullptr),
                  isA || isB);
        EXPECT_EQ((recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldB0Id()) != nullptr), isB);
        EXPECT_EQ((recordValue->tryGetValue(testDomain::TestRecordWithVariantsType::getFieldBCId()) != nullptr),
                  isB || isC);

        const unsigned int numChildren = 2 + (isC ? 1 : 3);

        EXPECT_EQ(recordType.getNumChildren(value), numChildren);

        std::vector<std::tuple<const babelwires::ValueHolder*, babelwires::PathStep, const babelwires::TypePtr&>>
            childInfos;
        std::vector<const babelwires::Type*> types;

        for (unsigned int i = 0; i < numChildren; ++i) {
            childInfos.emplace_back(recordType.getChild(value, i));
            types.emplace_back(std::get<2>(childInfos.back()).get());
        }

        const unsigned int fieldA0Index = 0;
        const unsigned int ff0Index = isA ? 1 : 0;
        const unsigned int fieldB0Index = 1;
        const unsigned int fieldABIndex = 2;
        const unsigned int fieldA1Index = 3;
        const unsigned int ff1Index = isA ? 4 : (isB ? 3 : 1);
        const unsigned int fieldBCIndex = isB ? 4 : 2;

        EXPECT_EQ(*std::get<1>(childInfos[ff0Index]).asField(), testDomain::TestRecordWithVariantsType::getFf0Id());
        EXPECT_EQ(*std::get<1>(childInfos[ff1Index]).asField(), testDomain::TestRecordWithVariantsType::getFf1Id());

        if (isA) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldA0Index]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldA0Id());
            EXPECT_EQ(*std::get<1>(childInfos[fieldABIndex]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldABId());
            EXPECT_EQ(*std::get<1>(childInfos[fieldA1Index]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldA1Id());
        }

        if (isB) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldB0Index]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldB0Id());
            EXPECT_EQ(*std::get<1>(childInfos[fieldABIndex]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldABId());
            EXPECT_EQ(*std::get<1>(childInfos[fieldBCIndex]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldBCId());
        }

        if (isC) {
            EXPECT_EQ(*std::get<1>(childInfos[fieldBCIndex]).asField(),
                      testDomain::TestRecordWithVariantsType::getFieldBCId());
        }

        for (unsigned int i = 0; i < numChildren; ++i) {
            EXPECT_EQ(recordType.getChildIndexFromStep(value, std::get<1>(childInfos[i])), i);
            EXPECT_TRUE(types[i]->isValidValue(typeSystem, *std::get<0>(childInfos[i])->getUnsafe()));
        }
    }
} // namespace

TEST(RecordWithVariantsTypeTest, traversal) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value,
                        testDomain::TestRecordWithVariantsType::getTagBId());

    recordType.selectTag(testEnvironment.m_typeSystem, value, testDomain::TestRecordWithVariantsType::getTagAId());
    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value,
                        testDomain::TestRecordWithVariantsType::getTagAId());

    recordType.selectTag(testEnvironment.m_typeSystem, value, testDomain::TestRecordWithVariantsType::getTagCId());
    verifyComplexRecord(testEnvironment.m_typeSystem, recordType, value,
                        testDomain::TestRecordWithVariantsType::getTagCId());
}

TEST(RecordWithVariantsTypeTest, getChildNonConstOfFixedField) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    const unsigned int ff1Index = 3;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, ff1Index)).asField(),
              testDomain::TestRecordWithVariantsType::getFf1Id());

    auto [value0, step0, type0] = recordType.getChild(value, ff1Index);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, ff1Index);

    EXPECT_EQ(*valueHolder0, **value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0->getTypeExp(), type1->getTypeExp());

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, ff1Index);
    EXPECT_EQ(*value2, valueHolder1);
}

TEST(RecordWithVariantsTypeTest, getChildNonConstOfFieldInBranch) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    recordType.selectTag(testEnvironment.m_typeSystem, value, testDomain::TestRecordWithVariantsType::getTagCId());

    const unsigned int fieldBCIndex = 2;

    EXPECT_EQ(*std::get<1>(recordType.getChild(value, fieldBCIndex)).asField(),
              testDomain::TestRecordWithVariantsType::getFieldBCId());

    auto [value0, step0, type0] = recordType.getChild(value, fieldBCIndex);

    babelwires::ValueHolder valueHolder0 = *value0;

    auto [value1, step1, type1] = recordType.getChildNonConst(value, fieldBCIndex);

    EXPECT_EQ(*valueHolder0, **value1);
    EXPECT_EQ(step0, step1);
    EXPECT_EQ(type0->getTypeExp(), type1->getTypeExp());

    *value1 = babelwires::IntValue(15);

    babelwires::ValueHolder valueHolder1 = *value1;

    EXPECT_NE(valueHolder0, valueHolder1);

    auto [value2, step2, type2] = recordType.getChild(value, fieldBCIndex);
    EXPECT_EQ(*value2, valueHolder1);
}

TEST(RecordWithVariantsTypeTest, subtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr recordVWithNoFields = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVWithNoFields>();
    const babelwires::TypePtr recordVA0 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVA0>();
    const babelwires::TypePtr recordVA1 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVA1>();
    const babelwires::TypePtr recordVAB = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVAB>();
    const babelwires::TypePtr recordVB = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVB>();
    const babelwires::TypePtr recordVAS = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVAS>();
    const babelwires::TypePtr recordVAV0 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVAV0>();
    const babelwires::TypePtr recordVABV0 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVABV0>();
    const babelwires::TypePtr recordVABV01 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVABV01>();
    const babelwires::TypePtr recordVAVB = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVAVB>();
    const babelwires::TypePtr recordVABV1 = testEnvironment.m_typeSystem.getRegisteredType<testDomain::RecordVABV1>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVWithNoFields, *recordVA0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVWithNoFields),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVA1),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA1, *recordVA0),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVAB),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVB, *recordVAB),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVB),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVB, *recordVA0),
              babelwires::SubtypeOrder::IsIntersecting);
    // Incompatible types
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAS, *recordVA0),
              babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVAS),
              babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAS, *recordVAB),
              babelwires::SubtypeOrder::IsDisjoint);

    // With variants
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVAV0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAV0, *recordVA0),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVABV0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV0, *recordVA0),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAB, *recordVABV0),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV0, *recordVAB),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV0, *recordVABV01),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV01, *recordVABV0),
              babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVA0, *recordVAVB),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAVB, *recordVA0),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAB, *recordVAVB),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVAVB, *recordVAB),
              babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV0, *recordVABV1),
              babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*recordVABV1, *recordVABV0),
              babelwires::SubtypeOrder::IsIntersecting);
}

TEST(RecordWithVariantsTypeTest, featureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_typeSystem,
                                           testDomain::TestRecordWithVariantsType::getThisIdentifier());
    valueFeature.setToDefault();

    const testDomain::TestRecordWithVariantsType* recordWithVariantsType =
        valueFeature.getType()->tryAs<testDomain::TestRecordWithVariantsType>();
    ASSERT_NE(recordWithVariantsType, nullptr);

    valueFeature.clearChanges();
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::SomethingChanged));
    {
        babelwires::ValueHolder value = valueFeature.getValue();
        recordWithVariantsType->selectTag(testEnvironment.m_typeSystem, value,
                                          testDomain::TestRecordWithVariantsType::getTagAId());
        valueFeature.setValue(value);
    }
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::ValueHolder value = valueFeature.getValue();
        recordWithVariantsType->selectTag(testEnvironment.m_typeSystem, value,
                                          testDomain::TestRecordWithVariantsType::getTagDId());
        valueFeature.setValue(value);
    }
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));

    valueFeature.clearChanges();
    {
        babelwires::Path pathToInt;
        pathToInt.pushStep(testDomain::TestRecordWithVariantsType::getFf1Id());
        pathToInt.pushStep(testDomain::TestSimpleRecordType::getInt0Id());
        valueFeature.setDescendentValue(pathToInt, babelwires::IntValue(15));
    }
    EXPECT_FALSE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::StructureChanged));
    EXPECT_TRUE(valueFeature.isChanged(babelwires::ValueTreeNode::Changes::ValueChanged));
}

TEST(RecordWithVariantsTypeTest, valueEquality) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder value0 = recordType.createValue(testEnvironment.m_typeSystem);
    babelwires::ValueHolder value1 = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value0);
    EXPECT_EQ(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value1, testDomain::TestRecordWithVariantsType::getTagAId());

    EXPECT_NE(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value0, testDomain::TestRecordWithVariantsType::getTagDId());

    EXPECT_NE(value0, value1);

    recordType.selectTag(testEnvironment.m_typeSystem, value0, testDomain::TestRecordWithVariantsType::getTagAId());

    EXPECT_EQ(value0, value1);
}

TEST(RecordWithVariantsTypeTest, valueHash) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestRecordWithVariantsType recordType(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const std::size_t hash0 = newValue->getHash();

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagAId());

    const std::size_t hash1 = newValue->getHash();

    EXPECT_NE(hash0, hash1);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testDomain::TestRecordWithVariantsType::getTagDId());

    const std::size_t hash2 = newValue->getHash();

    EXPECT_NE(hash0, hash2);
    EXPECT_NE(hash1, hash2);
}
