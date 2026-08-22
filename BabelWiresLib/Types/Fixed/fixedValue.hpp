/**
 * Holds a single value of fixed type.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <BabelWiresLib/Types/Fixed/fixed.hpp>

namespace babelwires {

    class BABELWIRESLIB_API FixedValue : public AlwaysEditableValue {
      public:
        DOWNCASTABLE(FixedValue, AlwaysEditableValue);
        CLONEABLE(FixedValue);
        SERIALIZABLE(FixedValue, "fixed", EditableValue, 1);

        FixedValue();
        FixedValue(Fixed value);

        const Fixed& get() const;
        void set(Fixed value);

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
        Fixed m_value;
    };

} // namespace babelwires
