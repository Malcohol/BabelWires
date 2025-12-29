#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Utilities/applyToSubvalues.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

class ApplyToSubvaluesTest : public ::testing::Test {
  protected:
    void SetUp() override {
        babelwires::TypeSystem& typeSystem = m_testEnvironment.m_typeSystem;

        // Use a ValueTreeNode and Instance as a convenient way to set up a value
        babelwires::ValueTreeRoot recordValueTreeNode(typeSystem,
                                                        testDomain::TestComplexRecordType::getThisIdentifier());
        testDomain::TestComplexRecordType::Instance recordInstance(recordValueTreeNode);
        recordInstance.getintR0().set(5);
        recordInstance.getintR1().set(-3);
        recordInstance.getrec().getintR0().set(10);
        recordInstance.getrec().getintR1().set(20);
        recordInstance.getarray().setSize(3);
        recordInstance.getarray().getEntry(0).set(1);
        recordInstance.getarray().getEntry(1).set(2);
        recordInstance.getarray().getEntry(2).set(3);
        recordInstance.activateAndGetonOpt().set(7);
        recordInstance.getstring().set("Hello");
        m_recordValue = recordValueTreeNode.getValue();
    }

    void testValues(int offset) {
        // Use a ValueTreeNode and Instance as a convenient way to test the value
        babelwires::TypeSystem& typeSystem = m_testEnvironment.m_typeSystem;
        babelwires::ValueTreeRoot recordValueTreeNode(typeSystem,
                                                        testDomain::TestComplexRecordType::getThisIdentifier());
        recordValueTreeNode.setValue(m_recordValue);
        testDomain::TestComplexRecordType::Instance recordInstance(recordValueTreeNode);
        EXPECT_EQ(recordInstance.getintR0().get(), 5 + offset);
        EXPECT_EQ(recordInstance.getintR1().get(), -3 + offset);
        EXPECT_EQ(recordInstance.getrec().getintR0().get(), 10 + offset);
        EXPECT_EQ(recordInstance.getrec().getintR1().get(), 20 + offset);
        EXPECT_EQ(recordInstance.getarray().getSize(), 3);
        EXPECT_EQ(recordInstance.getarray().getEntry(0).get(), 1 + offset);
        EXPECT_EQ(recordInstance.getarray().getEntry(1).get(), 2 + offset);
        EXPECT_EQ(recordInstance.getarray().getEntry(2).get(), 3 + offset);
        EXPECT_EQ(recordInstance.getonOpt().get(), 7 + offset); // Optional, but activated by default
        EXPECT_EQ(recordInstance.getstring().get(), "Hello");
    }

    testUtils::TestEnvironment m_testEnvironment;

    babelwires::ValueHolder m_recordValue;
};

TEST_F(ApplyToSubvaluesTest, applyToSubvalues) {
    babelwires::TypeSystem& typeSystem = m_testEnvironment.m_typeSystem;
    const auto& testComplexRecordType =
        typeSystem.getEntryByType<testDomain::TestComplexRecordType>();

    // Apply a function to increment all IntType subvalues by 1
    babelwires::applyToSubvalues(
        typeSystem, *testComplexRecordType, m_recordValue,
        [](const babelwires::Type& type, const babelwires::Value& value) { return type.as<babelwires::IntType>(); },
        [](const babelwires::Type& type, babelwires::Value& value) {
            auto& intValue = value.getAsEditableValue().is<babelwires::IntValue>();
            intValue.set(intValue.get() + 1);
        });
    // Check that all IntType subvalues have been incremented by 1
    testValues(1);
}

TEST_F(ApplyToSubvaluesTest, applyToSubvaluesOfType) {
    babelwires::TypeSystem& typeSystem = m_testEnvironment.m_typeSystem;
    const auto& testComplexRecordType =
        typeSystem.getEntryByType<testDomain::TestComplexRecordType>();

    // Apply a function to increment all IntType subvalues by 1
    babelwires::applyToSubvaluesOfType<babelwires::IntType>(
        typeSystem, *testComplexRecordType, m_recordValue,
        [](const babelwires::Type& type, babelwires::Value& value) {
            auto& intValue = value.getAsEditableValue().is<babelwires::IntValue>();
            intValue.set(intValue.get() + 1);
        });

    // Check that all IntType subvalues have been incremented by 1
    testValues(1);
}
