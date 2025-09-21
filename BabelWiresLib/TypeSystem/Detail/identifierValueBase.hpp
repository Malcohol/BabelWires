/**
 * The typeNameFormatter allows complex type names to be provided for type constructors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

namespace babelwires {

    template<typename IDENTIFIER>
    class IdentifierValueBase : public EditableValue {
      public:
        CLONEABLE_ABSTRACT(IdentifierValueBase);
        SERIALIZABLE_ABSTRACT(IdentifierValueBase, EditableValue);

        IdentifierValueBase() = default;
        IdentifierValueBase(IDENTIFIER value) : m_value(value) {}

        /// Get the current value of the feature.
        IDENTIFIER get() const { return m_value; }

        /// Set the value in the feature.
        void set(IDENTIFIER value) {
            m_value = value;
        }

        void serializeContents(Serializer& serializer) const override {
            serializer.serializeValue("value", m_value);
        }

        void deserializeContents(Deserializer& deserializer) override {
            deserializer.deserializeValue("value", m_value);
        }

        void visitIdentifiers(IdentifierVisitor& visitor) override { visitor(m_value); }
        void visitFilePaths(FilePathVisitor& visitor) override {}
        bool canContainIdentifiers() const override { return true; }
        bool canContainFilePaths() const override { return false; }

        std::string toString() const override {
            return IdentifierRegistry::read()->getName(m_value);
        }

      private:
        IDENTIFIER m_value;
    };

}
