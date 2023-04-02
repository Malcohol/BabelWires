#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#pragma once

namespace testUtils {
    class TestValue : public babelwires::Value {
      public:
        CLONEABLE(TestValue);
        SERIALIZABLE(TestValue, "testValue", babelwires::Value, 1);
        TestValue();
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;
        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override;
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override;
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;

        std::string m_value;
    };

    /// The Type of TestValues.
    class TestType : public babelwires::Type {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("TestType"), 1);

        babelwires::NewValueHolder createValue() const override;
        bool isValidValue(const babelwires::Value& value) const override;
    };
} // namespace testUtils
