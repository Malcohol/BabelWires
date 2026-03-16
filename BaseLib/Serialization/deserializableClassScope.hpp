/**
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Serialization/deserializationRegistry.hpp>

namespace babelwires {

    class OverlayDeserializationRegistry : public DeserializationRegistryInterface {
      public:
        explicit OverlayDeserializationRegistry(const DeserializationRegistryInterface& baseRegistry);

        template <typename T> void registerClass() {
            registerEntry(T::serializationType, T::getDeserializationRegistryEntry());
        }

        void registerEntry(std::string_view typeName, const Entry* entry);

        const Entry* findEntry(std::string_view typeName) const override;

      private:
        const DeserializationRegistryInterface& m_baseRegistry;
        std::map<std::string, const Entry*, std::less<>> m_registeredEntries;
    };

    class DeserializableClassScope {
      public:
        explicit DeserializableClassScope(Deserializer& deserializer);
        ~DeserializableClassScope();

        template <typename T> void registerClass() { m_overlayRegistry.registerClass<T>(); }

      private:
        Deserializer& m_deserializer;
        const DeserializationRegistryInterface* m_previousRegistry = nullptr;
        OverlayDeserializationRegistry m_overlayRegistry;
    };

} // namespace babelwires
