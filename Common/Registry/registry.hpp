#pragma once

#include "Common/exceptions.hpp"
#include "Common/types.hpp"

#include "Common/types.hpp"
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace babelwires {

    /// The expected base class of any entry stored in a registry.
    class RegistryEntry {
      public:
        RegistryEntry(std::string identifier, std::string name, VersionNumber version);
        virtual ~RegistryEntry();

        /// Return an identifier which uniquely identifies the entry.
        /// The identifier should never be changed.
        const std::string& getIdentifier() const;

        /// The name of the entry, which can be displayed to the user and is permitted to change
        /// over time.
        const std::string& getName() const;

        /// Get the version of this entry.
        VersionNumber getVersion() const;

      private:
        std::string m_identifier;
        std::string m_name;
        VersionNumber m_version;
    };

    /// The exception issued by Registry::getRegisteredEntry when an expected entry is not found.
    class RegistryException : public ExceptionWithStream<RegistryException> {};

    /// Provides the core functionality for a registry, but applies only to the RegistryEntry class.
    /// Since most use cases will want to register subclasses of RegistryEntry, they should use
    /// Registry, below.
    class UntypedRegistry {
      public:
        virtual ~UntypedRegistry() {}

        /// The registry name will be used in exceptions, and may be user visible.
        UntypedRegistry(std::string registryName);

        /// Transfer ownership to the registry.
        void addEntry(std::unique_ptr<RegistryEntry> newEntry);

        /// Find an entry by an internal key which should be stable between
        /// versions of the program.
        const RegistryEntry* getEntryByIdentifier(std::string_view identifier) const;

        /// Find an entry which is expected to be present.
        /// Will throw an RegistryException if the entry is not found.
        const RegistryEntry& getRegisteredEntry(std::string_view identifier) const;

      protected:
        virtual void validateNewEntry(const RegistryEntry* newEntry) const;
        const RegistryEntry* getEntryByName(std::string_view name) const;

      protected:
        std::string m_registryName;
        std::vector<std::unique_ptr<RegistryEntry>> m_entries;

      public:
        // Iteration.
        using Iterator = decltype(m_entries)::const_iterator;
        Iterator begin() const { return m_entries.begin(); }
        Iterator end() const { return m_entries.end(); }
    };

    /// Shared features of Format Registries.
    template <typename ENTRY, typename UNTYPED_REGISTRY = UntypedRegistry> class Registry {
      public:
        Registry(std::string registryName);

        /// Transfer ownership to the registry.
        void addEntry(std::unique_ptr<ENTRY> newEntry);

        /// Find an entry by an internal key which should be stable between
        /// versions of the program.
        const ENTRY* getEntryByIdentifier(std::string_view identifier) const;

        /// Find an entry which is expected to be present.
        /// Will throw an RegistryException if the entry is not found.
        const ENTRY& getRegisteredEntry(std::string_view identifier) const;

      public:
        class Iterator;
        // Iteration.
        Iterator begin() const;
        Iterator end() const;

      protected:
        UNTYPED_REGISTRY m_untypedRegistry;
    };

} // namespace babelwires

#include "Common/Registry/registry_inl.hpp"
