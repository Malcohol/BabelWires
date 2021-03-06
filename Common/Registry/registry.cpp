/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Registry/registry.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"

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

void babelwires::UntypedRegistry::addEntry(std::unique_ptr<RegistryEntry> newEntry) {
    assert((newEntry->getIdentifier().getDiscriminator() != 0) && "A registered entry must have a registered identifier");
    assert((getEntryByIdentifier(newEntry->getIdentifier()) == nullptr) &&
           "Format with that identifier already registered.");
    assert((getEntryByName(newEntry->getName()) == nullptr) && "Format with that name already registered.");
    validateNewEntry(newEntry.get());
    newEntry->onRegistered();
    m_entries.push_back(std::move(newEntry));
}

const babelwires::RegistryEntry* babelwires::UntypedRegistry::getEntryByIdentifier(const LongIdentifier& identifier) const {
    for (auto&& f : m_entries) {
        if (identifier == f->getIdentifier()) {
            // resolve the identifier if it is currently unresolved.
            f->getIdentifier().copyDiscriminatorTo(identifier);
            return f.get();
        }
    }
    return nullptr;
}

babelwires::RegistryEntry* babelwires::UntypedRegistry::getEntryByIdentifierNonConst(const LongIdentifier& identifier) const {
    for (auto&& f : m_entries) {
        if (identifier == f->getIdentifier()) {
            // resolve the identifier if it is currently unresolved.
            f->getIdentifier().copyDiscriminatorTo(identifier);
            return f.get();
        }
    }
    return nullptr;
}

const babelwires::RegistryEntry* babelwires::UntypedRegistry::getEntryByName(std::string_view name) const {
    for (auto&& f : m_entries) {
        if (name == f->getName()) {
            return f.get();
        }
    }
    return nullptr;
}

const babelwires::RegistryEntry& babelwires::UntypedRegistry::getRegisteredEntry(const LongIdentifier& identifier) const {
    const RegistryEntry* const entry = getEntryByIdentifier(identifier);
    if (!entry) {
        throw RegistryException() << "No entry called \"" << identifier << "\" was found in the " << m_registryName;
    }
    return *entry;
}

void babelwires::UntypedRegistry::validateNewEntry(RegistryEntry* newEntry) const {}
