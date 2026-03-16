/**
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/deserializableClassScope.hpp>

#include <BaseLib/Serialization/deserializer.hpp>

#include <cassert>

babelwires::OverlayDeserializationRegistry::OverlayDeserializationRegistry(
    const DeserializationRegistryInterface& baseRegistry)
    : m_baseRegistry(baseRegistry) {}

void babelwires::OverlayDeserializationRegistry::registerEntry(std::string_view typeName, const Entry* entry) {
    assert(entry && "Cannot register a null deserialization entry");
    const auto [it, wasInserted] = m_registeredEntries.insert(std::make_pair(std::string(typeName), entry));
    assert((wasInserted || (it->second == entry)) && "The same type was registered with different entries");
}

const babelwires::DeserializationRegistryInterface::Entry*
babelwires::OverlayDeserializationRegistry::findEntry(std::string_view typeName) const {
    const auto it = m_registeredEntries.find(typeName);
    if (it != m_registeredEntries.end()) {
        return it->second;
    }
    return m_baseRegistry.findEntry(typeName);
}


babelwires::DeserializableClassScope::DeserializableClassScope(Deserializer& deserializer)
    : m_deserializer(deserializer)
    , m_previousRegistry(&deserializer.getDeserializationRegistry())
    , m_overlayRegistry(*m_previousRegistry) {
    m_deserializer.setDeserializationRegistry(m_overlayRegistry);
}

babelwires::DeserializableClassScope::~DeserializableClassScope() {
    assert(m_previousRegistry && "The previous registry should always be set");
    m_deserializer.setDeserializationRegistry(*m_previousRegistry);
}
