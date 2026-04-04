/**
 * Holds a single value of rational type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <BaseLib/Math/rational.hpp>

namespace babelwires {

    class BABELWIRESLIB_API RationalValue : public AlwaysEditableValue {
      public:
        DOWNCASTABLE(RationalValue, AlwaysEditableValue);
        CLONEABLE(RationalValue);
        SERIALIZABLE(RationalValue, "rational", EditableValue, 1);

        RationalValue();
        RationalValue(Rational value);

        Rational get() const;
        void set(Rational value);

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
        Rational m_value;
    };

} // namespace babelwires
