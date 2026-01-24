#pragma once

#include <BaseLib/Serialization/automaticDeserializationRegistry.hpp>

#include <string>
#include <unordered_map>

namespace testUtils {
    /// Registry which extends AutomaticDeserializationRegistry with explicit registration.
    struct TestDeserializationRegistry : babelwires::AutomaticDeserializationRegistry {
        const Entry* findEntry(std::string_view typeName) const override;

        template <typename T> void registerEntry() {
            registerEntry(T::serializationType, T::getDeserializationRegistryEntry());
        }

        void registerEntry(std::string_view typeName, const Entry* entry);

        std::unordered_map<std::string, const Entry*> m_nameToEntry;
    };
} // namespace testUtils
