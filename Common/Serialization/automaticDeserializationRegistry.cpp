/**
 * The AutomaticDeserializationRegistry is a DeserializationRegistry into which serializable classes are automatically registered.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Serialization/automaticDeserializationRegistry.hpp"

#include <cassert>

namespace {

    /// This is zero initialized, so it will precede the construction of any of the AutomaticEntries.
    babelwires::AutomaticDeserializationRegistry::AutomaticEntry* g_entryListHead = 0;

    const babelwires::DeserializationRegistry::Entry* findEntryStatic(std::string_view typeName) {
        for (const babelwires::AutomaticDeserializationRegistry::AutomaticEntry* entry = g_entryListHead;
             entry != nullptr; entry = entry->m_next) {
            if (entry->m_serializationType == typeName) {
                return entry;
            }
            // The sortedness could be exploited here to early out in the fail case, but that case is not on the fast
            // path, so don't.
        }
        return nullptr;
    }

} // namespace

const babelwires::DeserializationRegistry::Entry*
babelwires::AutomaticDeserializationRegistry::findEntry(std::string_view typeName) const {
    return findEntryStatic(typeName);
}

babelwires::AutomaticDeserializationRegistry::AutomaticEntry::AutomaticEntry(Factory factory,
                                                                             std::string_view serializationType,
                                                                             VersionNumber version,
                                                                             const void* baseClassTag)
    : Entry{factory, serializationType, version, baseClassTag} {
    assert(!findEntryStatic(m_serializationType) && "The same type name was registered twice");

    // For ease of debugging, add the entry into a sorted position.
    babelwires::AutomaticDeserializationRegistry::AutomaticEntry** nextPtr = &g_entryListHead;
    while (*nextPtr && ((*nextPtr)->m_serializationType < serializationType)) {
        nextPtr = &((*nextPtr)->m_next);
    }

    m_next = *nextPtr;
    *nextPtr = this;
}
