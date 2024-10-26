#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Array/arrayValue.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ArrayTypeTest, simpleArrayTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestSimpleArrayType arrayType;

    EXPECT_EQ(arrayType.getEntryTypeStatic(), arrayType.getEntryType());
    EXPECT_EQ(arrayType.getInitialSize(), testUtils::TestSimpleArrayType::s_defaultSize);
    EXPECT_EQ(arrayType.getSizeRange().m_min, testUtils::TestSimpleArrayType::s_minimumSize);
    EXPECT_EQ(arrayType.getSizeRange().m_max, testUtils::TestSimpleArrayType::s_maximumSize);
    EXPECT_FALSE(arrayType.getKind().empty());

    babelwires::ValueHolder newValue = arrayType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newArrayValue = newValue->as<babelwires::ArrayValue>();
    EXPECT_NE(newArrayValue, nullptr);
    EXPECT_EQ(newArrayValue->getSize(), testUtils::TestSimpleArrayType::s_defaultSize);

    const babelwires::Type& entryType = arrayType.getEntryType().resolve(testEnvironment.m_typeSystem);
    const babelwires::ValueHolder defaultEntryValue = entryType.createValue(testEnvironment.m_typeSystem);

    for (int i = 0; i < newArrayValue->getSize(); ++i) {
        EXPECT_TRUE(entryType.isValidValue(testEnvironment.m_typeSystem, *newArrayValue->getValue(i)));
        // New entries start in default state
        EXPECT_EQ(defaultEntryValue, *newArrayValue->getValue(i));
    }
}

TEST(ArrayTypeTest, compoundArrayTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestCompoundArrayType arrayType;

    EXPECT_EQ(arrayType.getEntryTypeStatic(), arrayType.getEntryType());
    EXPECT_EQ(arrayType.getInitialSize(), testUtils::TestCompoundArrayType::s_defaultSize);
    EXPECT_EQ(arrayType.getSizeRange().m_min, testUtils::TestCompoundArrayType::s_minimumSize);
    EXPECT_EQ(arrayType.getSizeRange().m_max, testUtils::TestCompoundArrayType::s_maximumSize);
    EXPECT_FALSE(arrayType.getKind().empty());

    babelwires::ValueHolder newValue = arrayType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newArrayValue = newValue->as<babelwires::ArrayValue>();
    EXPECT_NE(newArrayValue, nullptr);
    EXPECT_EQ(newArrayValue->getSize(), testUtils::TestCompoundArrayType::s_defaultSize);

    const babelwires::Type& entryType = arrayType.getEntryType().resolve(testEnvironment.m_typeSystem);
    const babelwires::ValueHolder defaultEntryValue = entryType.createValue(testEnvironment.m_typeSystem);

    for (int i = 0; i < newArrayValue->getSize(); ++i) {
        EXPECT_TRUE(entryType.isValidValue(testEnvironment.m_typeSystem, *newArrayValue->getValue(i)));
        // New entries start in default state
        EXPECT_EQ(defaultEntryValue, *newArrayValue->getValue(i));
    }
}

TEST(ArrayTypeTest, isValidValueArrayCanBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects 0 entries to be allowed.
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_minimumSize, 0);

    testUtils::TestSimpleArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestSimpleArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    for (unsigned int i = testUtils::TestSimpleArrayType::s_minimumSize;
         i <= testUtils::TestSimpleArrayType::s_maximumSize; ++i) {
        babelwires::ArrayValue emptyValue(testEnvironment.m_typeSystem, entryType, i);
        EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, emptyValue));
    }

    babelwires::ArrayValue overFullValue(testEnvironment.m_typeSystem, entryType,
                                         testUtils::TestSimpleArrayType::s_maximumSize + 1);
    EXPECT_FALSE(arrayType.isValidValue(testEnvironment.m_typeSystem, overFullValue));

    babelwires::ArrayValue mixedValue(testEnvironment.m_typeSystem, entryType,
                                      testUtils::TestSimpleArrayType::s_maximumSize);

    // Make an entry not an element of the entry type.
    mixedValue.setValue(testUtils::TestSimpleArrayType::s_minimumSize, babelwires::StringValue("Not valid"));
    EXPECT_FALSE(arrayType.isValidValue(testEnvironment.m_typeSystem, mixedValue));

    // Restore the entry to a value of the entry type.
    mixedValue.setValue(testUtils::TestSimpleArrayType::s_minimumSize, babelwires::IntValue(12));
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, mixedValue));
}

TEST(ArrayTypeTest, isValidValueArrayCannotBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects at least 2 entries to be required
    EXPECT_GT(testUtils::TestCompoundArrayType::s_minimumSize, 1);

    testUtils::TestCompoundArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestCompoundArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    for (unsigned int i = testUtils::TestCompoundArrayType::s_minimumSize;
         i <= testUtils::TestCompoundArrayType::s_maximumSize; ++i) {
        babelwires::ArrayValue emptyValue(testEnvironment.m_typeSystem, entryType, i);
        EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, emptyValue));
    }

    babelwires::ArrayValue emptyArray(testEnvironment.m_typeSystem, entryType, 0);
    EXPECT_FALSE(arrayType.isValidValue(testEnvironment.m_typeSystem, emptyArray));

    babelwires::ArrayValue arrayTooSmall(testEnvironment.m_typeSystem, entryType,
                                         testUtils::TestCompoundArrayType::s_minimumSize - 1);
    EXPECT_FALSE(arrayType.isValidValue(testEnvironment.m_typeSystem, arrayTooSmall));
}

TEST(ArrayTypeTest, setSizeArrayCanBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects 0 entries to be allowed.
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_minimumSize, 0);
    // Test expects the array to have a flexible size
    EXPECT_NE(testUtils::TestSimpleArrayType::s_minimumSize, testUtils::TestSimpleArrayType::s_maximumSize);

    testUtils::TestSimpleArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestSimpleArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder valueHolder =
        babelwires::ValueHolder::makeValue<babelwires::ArrayValue>(testEnvironment.m_typeSystem, entryType, 0);

    // Careful: Mutating a valueHolder cause its held value to be replaced, so don't try to keep a reference to the
    // contained value.
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 0);

    arrayType.setSize(testEnvironment.m_typeSystem, valueHolder, 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 1);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.setSize(testEnvironment.m_typeSystem, valueHolder, 0);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 0);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(
        arrayType.setSize(testEnvironment.m_typeSystem, valueHolder, testUtils::TestSimpleArrayType::s_maximumSize + 1),
        babelwires::ModelException);
}

TEST(ArrayTypeTest, setSizeArrayCannotBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects at least 2 entries to be required
    EXPECT_GT(testUtils::TestCompoundArrayType::s_minimumSize, 1);
    // Test expects the array to have a flexible size
    EXPECT_NE(testUtils::TestCompoundArrayType::s_minimumSize, testUtils::TestCompoundArrayType::s_maximumSize);

    testUtils::TestCompoundArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestCompoundArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder valueHolder = babelwires::ValueHolder::makeValue<babelwires::ArrayValue>(
        testEnvironment.m_typeSystem, entryType, testUtils::TestCompoundArrayType::s_maximumSize);

    // Careful: Mutating a valueHolder cause its held value to be replaced, so don't try to keep a reference to the
    // contained value.
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), testUtils::TestCompoundArrayType::s_maximumSize);

    arrayType.setSize(testEnvironment.m_typeSystem, valueHolder, testUtils::TestCompoundArrayType::s_maximumSize - 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), testUtils::TestCompoundArrayType::s_maximumSize - 1);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.setSize(testEnvironment.m_typeSystem, valueHolder, testUtils::TestCompoundArrayType::s_maximumSize);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), testUtils::TestCompoundArrayType::s_maximumSize);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(arrayType.setSize(testEnvironment.m_typeSystem, valueHolder,
                                   testUtils::TestCompoundArrayType::s_minimumSize - 1),
                 babelwires::ModelException);
}

TEST(ArrayTypeTest, insertEntries) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects these sizes
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_minimumSize, 0);
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_maximumSize, 10);

    testUtils::TestSimpleArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestSimpleArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    babelwires::ArrayValue initialArray(testEnvironment.m_typeSystem, entryType, 4);
    for (unsigned int i = 0; i < 4; ++i) {
        initialArray.setValue(i, babelwires::IntValue(i + 1));
    }
    babelwires::ValueHolder valueHolder(initialArray);

    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 4);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.insertEntries(testEnvironment.m_typeSystem, valueHolder, 0, 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 5);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.insertEntries(testEnvironment.m_typeSystem, valueHolder, 2, 2);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 7);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(arrayType.insertEntries(testEnvironment.m_typeSystem, valueHolder, 5, 4), babelwires::ModelException);

    arrayType.insertEntries(testEnvironment.m_typeSystem, valueHolder, 7, 3);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 10);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));
    {
        const std::vector<unsigned int> expectedValues{0, 1, 0, 0, 2, 3, 4, 0, 0, 0};
        for (unsigned int i = 0; i < expectedValues.size(); ++i) {
            EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getValue(i), babelwires::IntValue(expectedValues[i]));
        }
    }

    EXPECT_THROW(arrayType.insertEntries(testEnvironment.m_typeSystem, valueHolder, 5, 1), babelwires::ModelException);
}

TEST(ArrayTypeTest, removeEntriesArrayCanBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects these sizes
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_minimumSize, 0);
    EXPECT_EQ(testUtils::TestSimpleArrayType::s_maximumSize, 10);

    testUtils::TestSimpleArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestSimpleArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    babelwires::ArrayValue initialArray(testEnvironment.m_typeSystem, entryType, 8);
    for (unsigned int i = 0; i < 8; ++i) {
        initialArray.setValue(i, babelwires::IntValue(i + 1));
    }
    babelwires::ValueHolder valueHolder(initialArray);

    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 8);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.removeEntries(valueHolder, 0, 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 7);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.removeEntries(valueHolder, 2, 2);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 5);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    arrayType.removeEntries(valueHolder, 3, 2);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 3);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));
    {
        const std::vector<unsigned int> expectedValues{2, 3, 6};
        for (unsigned int i = 0; i < expectedValues.size(); ++i) {
            EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getValue(i), babelwires::IntValue(expectedValues[i]));
        }
    }

    EXPECT_THROW(arrayType.removeEntries(valueHolder, 4, 1), babelwires::ModelException);
    EXPECT_THROW(arrayType.removeEntries(valueHolder, 3, 1), babelwires::ModelException);
    EXPECT_THROW(arrayType.removeEntries(valueHolder, 2, 2), babelwires::ModelException);
    EXPECT_THROW(arrayType.removeEntries(valueHolder, 0, 4), babelwires::ModelException);

    arrayType.removeEntries(valueHolder, 0, 3);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 0);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));
}

TEST(ArrayTypeTest, removeEntriesArrayCannotBeEmpty) {
    testUtils::TestEnvironment testEnvironment;

    // Test expects these sizes
    EXPECT_EQ(testUtils::TestCompoundArrayType::s_minimumSize, 2);
    EXPECT_EQ(testUtils::TestCompoundArrayType::s_maximumSize, 4);

    testUtils::TestCompoundArrayType arrayType;
    const babelwires::Type& entryType =
        testUtils::TestCompoundArrayType::getEntryTypeStatic().resolve(testEnvironment.m_typeSystem);

    babelwires::ArrayValue initialArray(testEnvironment.m_typeSystem, entryType, 4);
    babelwires::ValueHolder valueHolder(initialArray);

    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 4);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(arrayType.removeEntries(valueHolder, 0, 3), babelwires::ModelException);

    arrayType.removeEntries(valueHolder, 0, 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 3);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(arrayType.removeEntries(valueHolder, 0, 2), babelwires::ModelException);

    arrayType.removeEntries(valueHolder, 0, 1);
    EXPECT_EQ(valueHolder->is<babelwires::ArrayValue>().getSize(), 2);
    EXPECT_TRUE(arrayType.isValidValue(testEnvironment.m_typeSystem, *valueHolder));

    EXPECT_THROW(arrayType.removeEntries(valueHolder, 0, 1), babelwires::ModelException);
}

TEST(ArrayTypeTest, arrayTypeConstructorSucceed) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef arrayTypeRef(babelwires::ArrayTypeConstructor::getThisIdentifier(),
                                     babelwires::TypeConstructorArguments{
                                         {babelwires::StringType::getThisIdentifier()},
                                         {babelwires::IntValue(1), babelwires::IntValue(5), babelwires::IntValue(3)}});

    EXPECT_STREQ(arrayTypeRef.toString().c_str(), "Array<String>[1..5]");

    const babelwires::Type* const newType = arrayTypeRef.tryResolve(testEnvironment.m_typeSystem);
    ASSERT_NE(newType, nullptr);

    const babelwires::ArrayType* const arrayType = newType->as<babelwires::ArrayType>();
    ASSERT_NE(arrayType, nullptr);

    EXPECT_EQ(arrayType->getEntryType(), babelwires::StringType::getThisIdentifier());
    EXPECT_EQ(arrayType->getSizeRange().m_min, 1);
    EXPECT_EQ(arrayType->getSizeRange().m_max, 5);
    EXPECT_EQ(arrayType->getInitialSize(), 3);
}

TEST(ArrayTypeTest, arrayTypeConstructorFail) {
    testUtils::TestEnvironment testEnvironment;

    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::MediumId("NotAValidType")},
                {babelwires::IntValue(1), babelwires::IntValue(5), babelwires::IntValue(3)}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{{babelwires::StringType::getThisIdentifier()}, {}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::StringType::getThisIdentifier()},
                {babelwires::IntValue(-1), babelwires::IntValue(5), babelwires::IntValue(3)}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::StringType::getThisIdentifier()},
                {babelwires::IntValue(6), babelwires::IntValue(2), babelwires::IntValue(7)}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::StringType::getThisIdentifier()},
                {babelwires::IntValue(6), babelwires::IntValue(10), babelwires::IntValue(3)}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
    {
        babelwires::TypeRef arrayTypeRef(
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            babelwires::TypeConstructorArguments{
                {babelwires::StringType::getThisIdentifier()},
                {babelwires::IntValue(6), babelwires::IntValue(10), babelwires::IntValue(12)}});

        EXPECT_EQ(arrayTypeRef.tryResolve(testEnvironment.m_typeSystem), nullptr);
        EXPECT_THROW(arrayTypeRef.resolve(testEnvironment.m_typeSystem), babelwires::TypeSystemException);
    }
}

TEST(ArrayTypeTest, subtyping) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef arrayTypeRef(
        babelwires::ArrayTypeConstructor::getThisIdentifier(),
        babelwires::TypeConstructorArguments{
            {testUtils::TestSubEnum::getThisIdentifier()},
            {babelwires::IntValue(2), babelwires::IntValue(6), babelwires::IntValue(3)}});

    babelwires::TypeRef biggerArrayTypeRef(
        babelwires::ArrayTypeConstructor::getThisIdentifier(),
        babelwires::TypeConstructorArguments{
            {testUtils::TestSubEnum::getThisIdentifier()},
            {babelwires::IntValue(1), babelwires::IntValue(7), babelwires::IntValue(3)}});

    babelwires::TypeRef arrayOfSupertypeTypeRef(
        babelwires::ArrayTypeConstructor::getThisIdentifier(),
        babelwires::TypeConstructorArguments{
            {testUtils::TestEnum::getThisIdentifier()},
            {babelwires::IntValue(2), babelwires::IntValue(6), babelwires::IntValue(3)}});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(arrayTypeRef, arrayTypeRef), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(arrayTypeRef, biggerArrayTypeRef), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(biggerArrayTypeRef, arrayTypeRef), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(arrayTypeRef, arrayOfSupertypeTypeRef), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(arrayOfSupertypeTypeRef, arrayTypeRef), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(biggerArrayTypeRef, arrayOfSupertypeTypeRef), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(arrayOfSupertypeTypeRef, biggerArrayTypeRef), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(ArrayTypeTest, featureChanges) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SimpleValueFeature arrayFeature(testEnvironment.m_typeSystem, testUtils::TestSimpleArrayType::getThisIdentifier());
    arrayFeature.setToDefault();

    const testUtils::TestSimpleArrayType* arrayType = arrayFeature.getType().as<testUtils::TestSimpleArrayType>();
    ASSERT_NE(arrayType, nullptr);

    arrayFeature.clearChanges();
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    {
        babelwires::BackupScope scope(arrayFeature);
        babelwires::ValueHolder value = arrayFeature.getValue();
        arrayType->setSize(testEnvironment.m_typeSystem, value, testUtils::TestSimpleArrayType::s_nonDefaultSize);
        arrayFeature.setValue(value);
    }
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));

    arrayFeature.clearChanges();
    {
        babelwires::BackupScope scope(arrayFeature);
        babelwires::Path pathToInt;
        pathToInt.pushStep(babelwires::PathStep(1));
        babelwires::ValueHolder& value = arrayFeature.setModifiable(pathToInt);
        value = babelwires::IntValue(15);
    }
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
}

TEST(ArrayTypeTest, valueEquality) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestSimpleArrayType arrayType;

    babelwires::ValueHolder value0 = arrayType.createValue(testEnvironment.m_typeSystem);
    babelwires::ValueHolder value1 = arrayType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value0);
    EXPECT_EQ(value0, value1);

    arrayType.setSize(testEnvironment.m_typeSystem, value0, testUtils::TestSimpleArrayType::s_nonDefaultSize);
    EXPECT_NE(value0, value1);

    arrayType.setSize(testEnvironment.m_typeSystem, value1, testUtils::TestSimpleArrayType::s_nonDefaultSize);
    EXPECT_EQ(value0, value1);

    const auto& [childValue0, step0, typeRef0] = arrayType.getChildNonConst(value0, 2);
    *childValue0 = babelwires::IntValue(14);
    EXPECT_NE(value0, value1);

    const auto& [childValue1, step1, typeRef1] = arrayType.getChildNonConst(value1, 2);
    *childValue1 = babelwires::IntValue(14);
    EXPECT_EQ(value0, value1);
}

TEST(ArrayTypeTest, valueHash) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestSimpleArrayType arrayType;

    babelwires::ValueHolder value0 = arrayType.createValue(testEnvironment.m_typeSystem);
    babelwires::ValueHolder value1 = arrayType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value0);
    EXPECT_EQ(value0->getHash(), value1->getHash());

    arrayType.setSize(testEnvironment.m_typeSystem, value0, testUtils::TestSimpleArrayType::s_nonDefaultSize);
    EXPECT_NE(value0->getHash(), value1->getHash());

    arrayType.setSize(testEnvironment.m_typeSystem, value1, testUtils::TestSimpleArrayType::s_nonDefaultSize);
    EXPECT_EQ(value0->getHash(), value1->getHash());

    const auto& [childValue0, step0, typeRef0] = arrayType.getChildNonConst(value0, 2);
    *childValue0 = babelwires::IntValue(14);
    EXPECT_NE(value0->getHash(), value1->getHash());

    const auto& [childValue1, step1, typeRef1] = arrayType.getChildNonConst(value1, 2);
    *childValue1 = babelwires::IntValue(14);
    EXPECT_EQ(value0->getHash(), value1->getHash());
}

TEST(ArrayTypeTest, exceptions) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestCompoundArrayType arrayType;

    babelwires::ValueHolder value = arrayType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(value);

    EXPECT_THROW(arrayType.setSize(testEnvironment.m_typeSystem, value, testUtils::TestCompoundArrayType::s_maximumSize + 1), babelwires::ModelException);
    EXPECT_THROW(arrayType.setSize(testEnvironment.m_typeSystem, value, testUtils::TestCompoundArrayType::s_minimumSize - 1), babelwires::ModelException);

    EXPECT_NO_THROW(arrayType.setSize(testEnvironment.m_typeSystem, value, testUtils::TestCompoundArrayType::s_maximumSize));
    EXPECT_THROW(arrayType.insertEntries(testEnvironment.m_typeSystem, value, 0, 1), babelwires::ModelException);

    EXPECT_NO_THROW(arrayType.setSize(testEnvironment.m_typeSystem, value, testUtils::TestCompoundArrayType::s_minimumSize));
    EXPECT_THROW(arrayType.removeEntries(value, 0, 1), babelwires::ModelException);
}
