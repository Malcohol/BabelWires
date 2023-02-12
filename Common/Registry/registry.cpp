/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/Registry/registry.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::RegistryEntry::RegistryEntry(LongIdentifier identifier, VersionNumber version)
    : m_identifier(identifier)
    , m_version(version) {
    assert((version > 0) && "0 is not a valid version");
}

babelwires::RegistryEntry::~RegistryEntry() = default;

babelwires::LongIdentifier babelwires::RegistryEntry::getIdentifier() const {
    return m_identifier;
}

std::string babelwires::RegistryEntry::getName() const {
    return IdentifierRegistry::read()->getName(m_identifier);
}

babelwires::VersionNumber babelwires::RegistryEntry::getVersion() const {
    return m_version;
}

void babelwires::RegistryEntry::onRegistered() {}

babelwires::UntypedRegistry::UntypedRegistry(std::string registryName)
    : m_registryName(std::move(registryName)) {}

babelwires::RegistryEntry* babelwires::UntypedRegistry::addEntry(std::unique_ptr<RegistryEntry> newEntry) {
    LongIdentifier id = newEntry->getIdentifier();
    assert((newEntry->getIdentifier().getDiscriminator() != 0) &&
           "A registered entry must have a registered identifier");
    assert((getEntryByIdentifier(id) == nullptr) && "Format with that identifier already registered.");
    assert((getEntryByName(newEntry->getName()) == nullptr) && "Format with that name already registered.");
    RegistryEntry* const rawEntry = newEntry.get();
    newEntry->onRegistered();
    m_entries.insert({ id, std::move(newEntry) });
    return rawEntry;
}

const babelwires::RegistryEntry*
babelwires::UntypedRegistry::getEntryByIdentifier(const LongIdentifier& identifier) const {
    const auto& it = m_entries.find(identifier);
    if (it != m_entries.end()) {
        // resolve the identifier if it is currently unresolved.
        it->first.copyDiscriminatorTo(identifier);
        return it->second.get();
    }
    return nullptr;
}

babelwires::RegistryEntry*
babelwires::UntypedRegistry::getEntryByIdentifierNonConst(const LongIdentifier& identifier) const {
    const auto& it = m_entries.find(identifier);
    if (it != m_entries.end()) {
        // resolve the identifier if it is currently unresolved.
        it->first.copyDiscriminatorTo(identifier);
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
babelwires::UntypedRegistry::getRegisteredEntry(const LongIdentifier& identifier) const {
    const RegistryEntry* const entry = getEntryByIdentifier(identifier);
    if (!entry) {
        throw RegistryException() << "No entry called \"" << identifier << "\" was found in the " << m_registryName;
    }
    return *entry;
}
