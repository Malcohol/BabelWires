#include "Common/Registry/registry.hpp"

babelwires::RegistryEntry::RegistryEntry(std::string identifier, std::string name, VersionNumber version)
    : m_identifier(std::move(identifier))
    , m_name(std::move(name))
    , m_version(version) {
    assert((version > 0) && "0 is not a valid version");
}

babelwires::RegistryEntry::~RegistryEntry() = default;

const std::string& babelwires::RegistryEntry::getIdentifier() const {
    return m_identifier;
}

/// The name of the entry, which can be displayed to the user and is permitted to change.
const std::string& babelwires::RegistryEntry::getName() const {
    return m_name;
}

babelwires::VersionNumber babelwires::RegistryEntry::getVersion() const {
    return m_version;
}

babelwires::UntypedRegistry::UntypedRegistry(std::string registryName)
    : m_registryName(std::move(registryName)) {}

void babelwires::UntypedRegistry::addEntry(std::unique_ptr<RegistryEntry> newEntry) {
    assert((getEntryByIdentifier(newEntry->getIdentifier()) == nullptr) &&
           "Format with that identifier already registered.");
    assert((getEntryByName(newEntry->getName()) == nullptr) && "Format with that name already registered.");
    assert(isValidIdentifier(newEntry->getIdentifier().c_str()) && "Invalid identifier used for registry entry");
    validateNewEntry(newEntry.get());
    m_entries.push_back(std::move(newEntry));
}

const babelwires::RegistryEntry* babelwires::UntypedRegistry::getEntryByIdentifier(std::string_view identifier) const {
    for (auto&& f : m_entries) {
        if (identifier == f->getIdentifier()) {
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

const babelwires::RegistryEntry& babelwires::UntypedRegistry::getRegisteredEntry(std::string_view identifier) const {
    const RegistryEntry* const entry = getEntryByIdentifier(identifier);
    if (!entry) {
        throw RegistryException() << "No entry called \"" << identifier << "\" was found in the " << m_registryName;
    }
    return *entry;
}

void babelwires::UntypedRegistry::validateNewEntry(const RegistryEntry* newEntry) const {}
