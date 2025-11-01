/**
 * ListValue stores a sequence of elements of the same type.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Utilities/result.hpp>

#include <memory>
#include <vector>

namespace babelwires {
    class TypeSystem;
    class Type;

    /// The data held by a list of elements of the same type.
    class ListValue : public EditableValue {
      public:
        SERIALIZABLE(ListValue, "listValue", EditableValue, 1);
        CLONEABLE(ListValue);

        ListValue();
        ListValue(const ListValue& other);
        ListValue(ListValue&& other);
        ListValue(const TypeSystem& typeSystem, TypeRef elementRef);

        ListValue& operator=(const ListValue& other);
        ListValue& operator=(ListValue&& other);

        const TypeRef& getElementTypeRef() const;
        void setElementTypeRef(const TypeRef& elementRef);

        unsigned int getNumElements() const;
        const EditableValue& getElement(unsigned int index) const;
        void setElement(unsigned int index, std::unique_ptr<EditableValue> newElement);

        void clear();
        void pushBack(std::unique_ptr<EditableValue> newElement);

        bool operator==(const ListValue& other) const;
        bool operator==(const Value& other) const override;
        std::size_t getHash() const override;
        std::string toString() const override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool isValid(const TypeSystem& typeSystem) const;

      private:
        TypeRef m_elementTypeRef;
        std::vector<std::unique_ptr<EditableValue>> m_elements;
    };
} // namespace babelwires