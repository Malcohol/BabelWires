/**
 * Holds a single string.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {

    class StringValue : public Value {
      public:
        CLONEABLE(StringValue);
        SERIALIZABLE(StringValue, "string", Value, 1);

        StringValue();
        StringValue(std::string value);

        std::string get() const;
        void set(std::string value);

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;
        std::string toString() const override;

      private:
        std::string m_value;
    };

} // namespace babelwires
