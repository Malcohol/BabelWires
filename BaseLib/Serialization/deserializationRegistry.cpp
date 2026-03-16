/**
 * Concrete deserialization registry with explicit registration.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/deserializationRegistry.hpp>

#include <cassert>

void babelwires::DeserializationRegistry::registerEntry(std::string_view typeName, const Entry* entry) {
    assert(entry && "Cannot register a null deserialization entry");
    const auto [it, wasInserted] = m_registeredEntries.insert(std::make_pair(std::string(typeName), entry));
    assert((wasInserted || (it->second == entry)) && "The same type was registered with different entries");
}

const babelwires::DeserializationRegistryInterface::Entry*
babelwires::DeserializationRegistry::findEntry(std::string_view typeName) const {
    const auto it = m_registeredEntries.find(typeName);
    if (it != m_registeredEntries.end()) {
        return it->second;
    }
    return nullptr;
}

