/**
 * Concrete deserialization registry with explicit registration.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/explicitDeserializationRegistry.hpp>

#include <cassert>

void babelwires::ExplicitDeserializationRegistry::registerEntry(std::string_view typeName, const Entry* entry) {
    assert(entry && "Cannot register a null deserialization entry");
    const auto [it, wasInserted] = m_registeredEntries.insert(std::make_pair(std::string(typeName), entry));
    assert((wasInserted || (it->second == entry)) && "The same type was registered with different entries");
}

const babelwires::DeserializationRegistry::Entry*
babelwires::ExplicitDeserializationRegistry::findEntry(std::string_view typeName) const {
    const auto it = m_registeredEntries.find(typeName);
    if (it != m_registeredEntries.end()) {
        return it->second;
    }
    return nullptr;
}

babelwires::OverlayDeserializationRegistry::OverlayDeserializationRegistry(
    const DeserializationRegistry& baseRegistry)
    : m_baseRegistry(baseRegistry) {}

void babelwires::OverlayDeserializationRegistry::registerEntry(std::string_view typeName, const Entry* entry) {
    assert(entry && "Cannot register a null deserialization entry");
    const auto [it, wasInserted] = m_registeredEntries.insert(std::make_pair(std::string(typeName), entry));
    assert((wasInserted || (it->second == entry)) && "The same type was registered with different entries");
}

const babelwires::DeserializationRegistry::Entry*
babelwires::OverlayDeserializationRegistry::findEntry(std::string_view typeName) const {
    const auto it = m_registeredEntries.find(typeName);
    if (it != m_registeredEntries.end()) {
        return it->second;
    }
    return m_baseRegistry.findEntry(typeName);
}
