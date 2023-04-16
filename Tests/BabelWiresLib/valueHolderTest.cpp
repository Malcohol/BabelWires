#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

namespace {
    struct TestableValue : babelwires::Value {
        SERIALIZABLE(TestableValue, "TestableValue", Value, 1);
        CLONEABLE(TestableValue);

        TestableValue(int x = 0)
            : m_wasMoved(false)
            , m_wasCopied(false)
            , m_x(x) {}
        TestableValue(const TestableValue& other)
            : m_wasMoved(false)
            , m_wasCopied(true)
            , m_x(other.m_x) {}
        TestableValue(TestableValue&& other)
            : m_wasMoved(true)
            , m_wasCopied(false)
            , m_x(other.m_x) {}

        void serializeContents(babelwires::Serializer& serializer) const override {}
        void deserializeContents(babelwires::Deserializer& deserializer) override {}
        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override {}
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override {}
        bool canContainIdentifiers() const override { return false; }
        bool canContainFilePaths() const override { return false; }
        std::size_t getHash() const override { return 0; }
        bool operator==(const Value& other) const override { return true; }
        std::string toString() const override { return "TestValue"; }

      public:
        bool m_wasMoved;
        bool m_wasCopied;
        int m_x;
    };
} // namespace

TEST(ValueHolderTest, constructionFromRValue) {
    babelwires::ValueHolder valueHolder{TestableValue(5)};
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_TRUE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, constructionFromLValue) {
    TestableValue lvalue(5);
    babelwires::ValueHolder valueHolder{lvalue};
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_TRUE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, constructionFromSharedPtr) {
    auto sharedPtr = std::make_shared<TestableValue>(5);
    babelwires::ValueHolder valueHolder{sharedPtr};
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, constructionFromUniquePtr) {
    std::unique_ptr<babelwires::Value> uniquePtr(new TestableValue(5));
    babelwires::ValueHolder valueHolder{std::move(uniquePtr)};
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, assignmentFromRValue) {
    babelwires::ValueHolder valueHolder;
    valueHolder = TestableValue(5);
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_TRUE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, assignmentFromLValue) {
    TestableValue lvalue(5);
    babelwires::ValueHolder valueHolder;
    valueHolder = lvalue;
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_TRUE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, assignmentFromSharedPtr) {
    auto sharedPtr = std::make_shared<TestableValue>(5);
    babelwires::ValueHolder valueHolder;
    valueHolder = sharedPtr;
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, assignmentFromUniquePtr) {
    std::unique_ptr<babelwires::Value> uniquePtr(new TestableValue(5));
    babelwires::ValueHolder valueHolder;
    valueHolder = std::move(uniquePtr);
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, makeValue) {
    babelwires::NewValueHolder newValueHolder = babelwires::ValueHolder::makeValue<TestableValue>(15);

    TestableValue* const nonConstValue = newValueHolder.m_nonConstReference.as<TestableValue>();
    EXPECT_NE(nonConstValue, nullptr);
    EXPECT_EQ(nonConstValue->m_x, 15);
    nonConstValue->m_x = -3;

    babelwires::ValueHolder valueHolder = std::move(newValueHolder);
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);
    EXPECT_EQ(valueInHolder->m_x, -3);
}

