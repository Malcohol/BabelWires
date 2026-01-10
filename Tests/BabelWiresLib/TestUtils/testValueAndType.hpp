#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#pragma once

namespace testUtils {
    class TestValue : public babelwires::EditableValue {
      public:
        CLONEABLE(TestValue);
        SERIALIZABLE(TestValue, "testValue", babelwires::EditableValue, 1);
        TestValue(std::string value = "The value");
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
        REGISTERED_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("TestType"), 1);

        /// 0 == unbounded.
        TestType(unsigned int maximumLength = 0, std::string defaultValue = "Default value");
        
        TestType(babelwires::TypeExp&& typeExpOfThis, unsigned int maximumLength = 0, std::string defaultValue = "Default value");

        babelwires::NewValueHolder createValue(const babelwires::TypeSystem& typeSystem) const override;
        bool visitValue(const babelwires::TypeSystem& typeSystem, const babelwires::Value& value,
                                ChildValueVisitor& visitor) const override;
        std::string getFlavour() const override;

        std::optional<babelwires::SubtypeOrder> compareSubtypeHelper(const babelwires::TypeSystem& typeSystem, const babelwires::Type& other) const override;

        std::string valueToString(const babelwires::TypeSystem& typeSystem, const babelwires::ValueHolder& v) const override;

        /// The TestType is tagged with this tag.
        static Tag getTestTypeTag();

        unsigned int m_maximumLength;
        std::string m_defaultValue;
    };
} // namespace testUtils
