/**
 * Holds a single value of integer type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>

namespace babelwires {

    class IntValue : public AlwaysEditableValue {
      public:
        CLONEABLE(IntValue);
        SERIALIZABLE(IntValue, "int", EditableValue, 1);

        using NativeType = std::int64_t; 

        IntValue();
        IntValue(NativeType value);

        NativeType get() const;
        void set(NativeType value);

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;

      private:
        NativeType m_value;
    };

} // namespace babelwires
