#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <BaseLib/Text/text.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#pragma once

namespace testUtils {
    class TestValue : public babelwires::EditableValue {
      public:
        DOWNCASTABLE(TestValue, babelwires::EditableValue);
        CLONEABLE(TestValue);
        SERIALIZABLE(TestValue, "testValue", babelwires::EditableValue, 1);
        TestValue(babelwires::Text value = u8"The value");
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;
        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override;
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override;
        void serializeContents(babelwires::Serializer& serializer) const override;
        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;

        babelwires::Text m_value;
    };

    /// The Type of TestValues.
    class TestType : public babelwires::Type {
      public:
        DOWNCASTABLE(TestType, babelwires::Type);
        REGISTERED_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("TestType"), 1);

        /// 0 == unbounded.
        TestType(unsigned int maximumLength = 0, babelwires::Text defaultValue = u8"Default value");
        
        TestType(babelwires::TypeExp&& typeExpOfThis, unsigned int maximumLength = 0, babelwires::Text defaultValue = u8"Default value");

        babelwires::NewValueHolder createValue(const babelwires::TypeSystem& typeSystem) const override;
        bool visitValue(const babelwires::TypeSystem& typeSystem, const babelwires::Value& value,
                                ChildValueVisitor& visitor) const override;
        std::string getFlavour() const override;

        std::optional<babelwires::SubtypeOrder> compareSubtypeHelper(const babelwires::TypeSystem& typeSystem, const babelwires::Type& other) const override;

        std::string valueToString(const babelwires::TypeSystem& typeSystem, const babelwires::ValueHolder& v) const override;

        /// The TestType is tagged with this tag.
        static Tag getTestTypeTag();

        unsigned int m_maximumLength;
        babelwires::Text m_defaultValue;
    };
} // namespace testUtils
