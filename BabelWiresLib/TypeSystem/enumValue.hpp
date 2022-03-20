/**
 * TODO
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
        EnumValue(Identifier value);

        /// Get the current value of the feature.
        Identifier get() const;

        /// Set the value in the feature.
        void set(Identifier value);

        bool isValid(const TypeSystem& typeSystem, LongIdentifier type) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        std::size_t getHash() const override;

      private:
        Identifier m_value;
    };
} // namespace babelwires
