/**
 * Temporarily register a class for deserialization.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Serialization/deserializationRegistry.hpp>
#include <BaseLib/Serialization/deserializer.hpp>

namespace babelwires {

    /// Temporarily register a class for deserialization.
    /// This is for use when instances of a locally-defined class may be deserialized.
    /// Most classes should be registered in the global registry and that _must_ be true of
    /// any class that is unknown to the deserializing code (e.g. a subclass coming from
    /// a plugin).
    template <typename T> class DeserializableClassScope final : public DeserializationRegistryInterface {
      public:
        explicit DeserializableClassScope(Deserializer& deserializer)
            : m_deserializer(deserializer)
            , m_previousRegistry(&deserializer.getDeserializationRegistry()) {
            m_deserializer.setDeserializationRegistry(*this);
        }

        ~DeserializableClassScope() { m_deserializer.setDeserializationRegistry(*m_previousRegistry); }

        const Entry* findEntry(std::string_view typeName) const override {
            if (typeName == T::serializationType) {
                return T::getDeserializationRegistryEntry();
            }
            return m_previousRegistry->findEntry(typeName);
        }

      private:
        Deserializer& m_deserializer;
        const DeserializationRegistryInterface* m_previousRegistry = nullptr;
    };

} // namespace babelwires
