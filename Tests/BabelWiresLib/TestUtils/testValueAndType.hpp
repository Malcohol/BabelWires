#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#pragma once

namespace testUtils {
    class TestValue : public babelwires::Value {
      public:
        CLONEABLE(TestValue);
        SERIALIZABLE(TestValue, "testValue", void, 1);
        bool isValid(const babelwires::Type& type) const override;
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;
        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override;
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override;
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;

        std::string m_value;
    };

    class TestType : public babelwires::Type {
      public:
        TestType();

        static babelwires::LongIdentifier getThisIdentifier();

        std::unique_ptr<babelwires::Value> createValue() const override;
    };
} // namespace testUtils
