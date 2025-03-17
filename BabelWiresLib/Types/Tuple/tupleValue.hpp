/**
 * TupleValues are instances of TupleTypes.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <vector>

namespace babelwires {
    class TupleValue : public EditableValue {
      public:
        CLONEABLE(TupleValue);
        SERIALIZABLE(TupleValue, "tuple", EditableValue, 1);

        using Tuple = std::vector<EditableValueHolder>;

        TupleValue(Tuple values);

        unsigned int getSize() const;
        EditableValueHolder& getValue(unsigned int index);
        const EditableValueHolder& getValue(unsigned int index) const;
        void setValue(unsigned int index, EditableValueHolder newValue);

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;
        std::string toString() const override;

      private:
        TupleValue() = default;

      private:
        std::vector<EditableValueHolder> m_componentValues;
    };

} // namespace babelwires
