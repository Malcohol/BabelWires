/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/Registry/registry.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::RegistryEntry::RegistryEntry(LongId identifier, VersionNumber version)
    : m_identifier(identifier)
    , m_version(version) {
    assert((version > 0) && "0 is not a valid version");
}

babelwires::RegistryEntry::~RegistryEntry() = default;

babelwires::LongId babelwires::RegistryEntry::getIdentifier() const {
    return m_identifier;
}

std::string babelwires::RegistryEntry::getName() const {
    return IdentifierRegistry::read()->getName(m_identifier);
}

babelwires::VersionNumber babelwires::RegistryEntry::getVersion() const {
    return m_version;
}

babelwires::UntypedRegistry::UntypedRegistry(std::string registryName)
    : m_registryName(std::move(registryName)) {}

babelwires::RegistryEntry* babelwires::UntypedRegistry::addEntry(std::unique_ptr<RegistryEntry> newEntry) {
    LongId id = newEntry->getIdentifier();
    assert((newEntry->getIdentifier().getDiscriminator() != 0) &&
           "A registered entry must have a registered identifier");
    assert((getEntryByIdentifier(id) == nullptr) && "Entry with that identifier already registered.");
    assert((getEntryByName(newEntry->getName()) == nullptr) && "Entry with that name already registered.");
    RegistryEntry* const rawEntry = newEntry.get();
    m_entries.insert({ id, std::move(newEntry) });
    return rawEntry;
}

const babelwires::RegistryEntry*
babelwires::UntypedRegistry::getEntryByIdentifier(const LongId& identifier) const {
    const auto& it = m_entries.find(identifier);
    if (it != m_entries.end()) {
        return it->second.get();
    }
    return nullptr;
}

const babelwires::RegistryEntry*
babelwires::UntypedRegistry::getEntryByIdentifierAndResolve(LongId& identifier) const {
    if (const auto* entry = getEntryByIdentifier(identifier)) {
        identifier = entry->getIdentifier();
        return entry;
    }
    return nullptr;
}

babelwires::RegistryEntry*
babelwires::UntypedRegistry::getEntryByIdentifierNonConst(const LongId& identifier) const {
    const auto& it = m_entries.find(identifier);
    if (it != m_entries.end()) {
        return it->second.get();
    }
    return nullptr;
}

const babelwires::RegistryEntry* babelwires::UntypedRegistry::getEntryByName(std::string_view name) const {
    auto identifierRegistryScope = IdentifierRegistry::read();
    for (auto&& it : m_entries) {
        if (name == identifierRegistryScope->getName(it.first)) {
            return it.second.get();
        }
    }
    return nullptr;
}

const babelwires::RegistryEntry&
babelwires::UntypedRegistry::getRegisteredEntry(const LongId& identifier) const {
    const RegistryEntry* const entry = getEntryByIdentifier(identifier);
    if (!entry) {
        throw RegistryException() << "No entry called \"" << identifier << "\" was found in the " << m_registryName;
    }
    return *entry;
}
