/**
 * Holds a single text value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <BaseLib/Text/text.hpp>

namespace babelwires {

    class BABELWIRESLIB_API TextValue : public AlwaysEditableValue {
      public:
        DOWNCASTABLE(TextValue, AlwaysEditableValue);
        CLONEABLE(TextValue);
        SERIALIZABLE(TextValue, "text", EditableValue, 1);

        TextValue();
        TextValue(Text value);

        Text get() const;
        void set(Text value);

        void serializeContents(Serializer& serializer) const override;
        Result deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;

      private:
        Text m_value;
    };

} // namespace babelwires
