#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Common/DataContext/filePath.hpp>

namespace {
    struct TestableValue : babelwires::AlwaysEditableValue {
        SERIALIZABLE(TestableValue, "TestableValue", Value, 1);
        CLONEABLE(TestableValue);

        TestableValue(int x = 0, bool canContainIdentifiers = false, bool canContainFilePaths = false)
            : m_wasMoved(false)
            , m_wasCopied(false)
            , m_x(x)
            , m_canContainIdentifiers(canContainIdentifiers)
            , m_canContainFilePaths(canContainFilePaths) {}
        TestableValue(const TestableValue& other)
            : m_wasMoved(false)
            , m_wasCopied(true)
            , m_x(other.m_x)
            , m_canContainIdentifiers(other.m_canContainIdentifiers)
            , m_canContainFilePaths(other.m_canContainFilePaths) {}
        TestableValue(TestableValue&& other)
            : m_wasMoved(true)
            , m_wasCopied(false)
            , m_x(other.m_x)
            , m_canContainIdentifiers(other.m_canContainIdentifiers)
            , m_canContainFilePaths(other.m_canContainFilePaths) {}

        void serializeContents(babelwires::Serializer& serializer) const override {}
        void deserializeContents(babelwires::Deserializer& deserializer) override {}
        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override {
            if (m_canContainIdentifiers) {
                visitor(m_id);
            }
        }
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override {
            if (m_canContainFilePaths) {
                visitor(m_filePath);
            }
        }
        bool canContainIdentifiers() const override { return m_canContainIdentifiers; }
        bool canContainFilePaths() const override { return m_canContainFilePaths; }
        std::size_t getHash() const override { return 0; }
        bool operator==(const Value& other) const override {
            const TestableValue* const otherTestableValue = other.as<TestableValue>();
            return !otherTestableValue || (m_x == otherTestableValue->m_x);
        }
        std::string toString() const override { return "TestValue"; }

      public:
        bool m_wasMoved;
        bool m_wasCopied;
        int m_x = 0;
        bool m_canContainIdentifiers = false;
        bool m_canContainFilePaths = false;
        babelwires::ShortId m_id = "Foo";
        babelwires::FilePath m_filePath = std::filesystem::path("Bar");
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

TEST(ValueHolderTest, constructionFromUniquePtr) {
    auto uniquePtr = std::make_unique<TestableValue>(5);
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

TEST(ValueHolderTest, assignmentFromUniquePtr) {
    auto uniquePtr = std::make_unique<TestableValue>(5);
    babelwires::ValueHolder valueHolder;
    valueHolder = std::move(uniquePtr);
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);

    EXPECT_FALSE(valueInHolder->m_wasCopied);
    EXPECT_FALSE(valueInHolder->m_wasMoved);
    EXPECT_EQ(valueInHolder->m_x, 5);
}

TEST(ValueHolderTest, makeValue) {
    auto newValueHolder = babelwires::ValueHolder::makeValue<TestableValue>(15);

    EXPECT_EQ(newValueHolder.m_nonConstReference.m_x, 15);
    newValueHolder.m_nonConstReference.m_x = -3;

    babelwires::ValueHolder valueHolder = std::move(newValueHolder);
    const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
    ASSERT_NE(valueInHolder, nullptr);
    EXPECT_EQ(valueInHolder->m_x, -3);
}

TEST(ValueHolderTest, copyContentsAndGetNonConst) {
    babelwires::ValueHolder valueHolder{TestableValue(5)};
    babelwires::ValueHolder valueHolder2{valueHolder};
    babelwires::ValueHolder valueHolder3{valueHolder};
    {
        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        ASSERT_NE(valueInHolder, nullptr);
        EXPECT_EQ(valueInHolder->m_x, 5);
    }
    {
        const TestableValue* const valueInHolder2 = valueHolder2->as<TestableValue>();
        ASSERT_NE(valueInHolder2, nullptr);
        EXPECT_EQ(valueInHolder2->m_x, 5);
    }
    {
        const TestableValue* const valueInHolder3 = valueHolder3->as<TestableValue>();
        ASSERT_NE(valueInHolder3, nullptr);
        EXPECT_EQ(valueInHolder3->m_x, 5);
    }
    {
        babelwires::Value& copyOf2 = valueHolder2.copyContentsAndGetNonConst();
        TestableValue& valueInHolder2NonConst = dynamic_cast<TestableValue&>(copyOf2);
        valueInHolder2NonConst.m_x = 23;

        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        const TestableValue* const valueInHolder2 = valueHolder2->as<TestableValue>();
        const TestableValue* const valueInHolder3 = valueHolder3->as<TestableValue>();
        EXPECT_EQ(valueInHolder->m_x, 5);
        EXPECT_EQ(valueInHolder2->m_x, 23);
        EXPECT_EQ(valueInHolder3->m_x, 5);
    }
}

TEST(ValueHolderTest, equality) {
    babelwires::ValueHolder valueHolderEmpty;
    babelwires::ValueHolder valueHolderEmpty2;
    babelwires::ValueHolder valueHolderFive{TestableValue(5)};
    babelwires::ValueHolder valueHolderFive2{TestableValue(5)};
    babelwires::ValueHolder valueHolderSeven{TestableValue(7)};

    EXPECT_TRUE(valueHolderEmpty == valueHolderEmpty);
    EXPECT_TRUE(valueHolderEmpty == valueHolderEmpty2);
    EXPECT_TRUE(valueHolderFive == valueHolderFive);
    EXPECT_TRUE(valueHolderFive == valueHolderFive2);

    EXPECT_FALSE(valueHolderEmpty == valueHolderFive);
    EXPECT_FALSE(valueHolderFive == valueHolderSeven);

    EXPECT_FALSE(valueHolderEmpty != valueHolderEmpty);
    EXPECT_FALSE(valueHolderEmpty != valueHolderEmpty2);
    EXPECT_FALSE(valueHolderFive != valueHolderFive);
    EXPECT_FALSE(valueHolderFive != valueHolderFive2);

    EXPECT_TRUE(valueHolderEmpty != valueHolderFive);
    EXPECT_TRUE(valueHolderFive != valueHolderSeven);
}

TEST(ValueHolderTest, visitIdentifiers) {
    struct IdentifierVisitor : babelwires::IdentifierVisitor {
        virtual void operator()(babelwires::ShortId& identifier) {
            m_ids.emplace_back(identifier);
            identifier = "Foo2";
        }
        virtual void operator()(babelwires::MediumId& identifier) {}
        virtual void operator()(babelwires::LongId& identifier) {}

        std::vector<babelwires::ShortId> m_ids;
    };

    {
        bool hasIdentifiers = false;
        babelwires::ValueHolder valueHolder{TestableValue(5, hasIdentifiers)};
        IdentifierVisitor visitor;
        valueHolder.visitIdentifiers(visitor);
        EXPECT_EQ(visitor.m_ids.size(), 0);
        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        ASSERT_NE(valueInHolder, nullptr);
        EXPECT_EQ(valueInHolder->m_id, "Foo");
    }
    {
        bool hasIdentifiers = true;
        babelwires::ValueHolder valueHolder{TestableValue(5, hasIdentifiers)};
        IdentifierVisitor visitor;
        valueHolder.visitIdentifiers(visitor);
        EXPECT_EQ(visitor.m_ids.size(), 1);
        EXPECT_EQ(visitor.m_ids[0], "Foo");
        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        ASSERT_NE(valueInHolder, nullptr);
        EXPECT_EQ(valueInHolder->m_id, "Foo2");
    }
}

TEST(ValueHolderTest, visitFilePaths) {
    std::vector<babelwires::FilePath> filePaths;

    babelwires::FilePathVisitor filePathVisitor = [&filePaths] (babelwires::FilePath& filePath) {
        filePaths.emplace_back(filePath);
        filePath = std::filesystem::path("Bar2");
    };

    {
        bool hasFilePaths = false;
        babelwires::ValueHolder valueHolder{TestableValue(5, false, hasFilePaths)};
        valueHolder.visitFilePaths(filePathVisitor);
        EXPECT_EQ(filePaths.size(), 0);
        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        ASSERT_NE(valueInHolder, nullptr);
        EXPECT_EQ(valueInHolder->m_filePath, babelwires::FilePath(std::filesystem::path("Bar")));
    }
    {
        bool hasFilePaths = true;
        babelwires::ValueHolder valueHolder{TestableValue(5, false, hasFilePaths)};
        valueHolder.visitFilePaths(filePathVisitor);
        EXPECT_EQ(filePaths.size(), 1);
        EXPECT_EQ(filePaths[0], babelwires::FilePath(std::filesystem::path("Bar")));
        const TestableValue* const valueInHolder = valueHolder->as<TestableValue>();
        ASSERT_NE(valueInHolder, nullptr);
        EXPECT_EQ(valueInHolder->m_filePath, babelwires::FilePath(std::filesystem::path("Bar2")));
    }
}
