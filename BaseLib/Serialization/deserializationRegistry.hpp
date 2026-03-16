/**
 * Concrete deserialization registry with explicit registration.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Serialization/deserializationRegistryInterface.hpp>

#include <map>
#include <string>

namespace babelwires {

    class DeserializationRegistry : public DeserializationRegistryInterface {
      public:
        template <typename T> void registerClass();

        void registerEntry(std::string_view typeName, const Entry* entry);

        const Entry* findEntry(std::string_view typeName) const override;

      private:
        std::map<std::string, const Entry*, std::less<>> m_registeredEntries;
    };



} // namespace babelwires

template <typename T>
void babelwires::DeserializationRegistry::registerClass() {
    registerEntry(T::serializationType, T::getDeserializationRegistryEntry());
}
