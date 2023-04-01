/**
 * Holds a single value of an enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {

    class EnumValue : public Value {
      public:
        CLONEABLE(EnumValue);
        SERIALIZABLE(EnumValue, "enum", Value, 1);

        EnumValue();
        EnumValue(ShortId value);

        /// Get the current value of the feature.
        ShortId get() const;

        /// Set the value in the feature.
        void set(ShortId value);

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
        ShortId m_value;
    };
} // namespace babelwires
