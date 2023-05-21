/**
 * Holds a single value of sum type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {

    class SumValue : public EditableValue {
      public:
        CLONEABLE(SumValue);
        SERIALIZABLE(SumValue, "summand", EditableValue, 1);

        SumValue();
        SumValue(TypeRef typeTag, EditableValueHolder value);

        const TypeRef& getTypeTag() const;
        const EditableValueHolder& getValue() const;
        void set(TypeRef typeTag, EditableValueHolder value);

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
        TypeRef m_typeTag;
        EditableValueHolder m_value;
    };

} // namespace babelwires
