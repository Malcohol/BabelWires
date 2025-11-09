/**
 * Holds a single value of rational type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <Common/Math/rational.hpp>

namespace babelwires {

    class RationalValue : public AlwaysEditableValue {
      public:
        CLONEABLE(RationalValue);
        SERIALIZABLE(RationalValue, "rational", EditableValue, 1);

        RationalValue();
        RationalValue(Rational value);

        Rational get() const;
        void set(Rational value);

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
        Rational m_value;
    };

} // namespace babelwires
