/**
 * A Registry is a container which is used in various places for registering factories.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/exceptions.hpp>
//#include <Common/types.hpp>
#include <Common/Identifiers/identifier.hpp>

#include <Common/types.hpp>
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace babelwires {

    /// The expected base class of any entry stored in a registry.
    class RegistryEntry {
      public:
        /// An identifier is used to uniquely identify the entry.
        /// Typically, it will be obtained via the REGISTERED_LONGID macro.
        RegistryEntry(LongIdentifier identifier, VersionNumber version);
        virtual ~RegistryEntry();

        /// Return an identifier which uniquely identifies the entry.
        /// The identifier should never be changed.
        LongIdentifier getIdentifier() const;

        /// The name of the entry, which can be displayed to the user and is permitted to change
        /// over time.
        std::string getName() const;

        /// Get the version of this entry.
        VersionNumber getVersion() const;

        /// Called when an entry gets registered. The default implementation does nothing.
        virtual void onRegistered();

      private:
        LongIdentifier m_identifier;
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
        RegistryEntry* addEntry(std::unique_ptr<RegistryEntry> newEntry);

        /// Find an entry by an internal key which should be stable between versions of the program.
        /// If the provided identifier is unresolved, it will be resolved by setting its (mutable) discriminator to
        /// match that of the registered entry. Care should be taken to ensure the reference is not a temporary.
        const RegistryEntry* getEntryByIdentifier(const LongIdentifier& identifier) const;

        /// Find an entry which is expected to be present.
        /// Will throw an RegistryException if the entry is not found.
        /// If the provided identifier is unresolved, it will be resolved by setting its (mutable) discriminator to
        /// match that of the registered entry. Care should be taken to ensure the reference is not a temporary.
        const RegistryEntry& getRegisteredEntry(const LongIdentifier& identifier) const;

      protected:
        virtual void validateNewEntry(RegistryEntry* newEntry) const;
        const RegistryEntry* getEntryByName(std::string_view name) const;

        /// Protected non-const version available to subclasses.
        RegistryEntry* getEntryByIdentifierNonConst(const LongIdentifier& identifier) const;

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
        template<typename ENTRY_SUBTYPE, std::enable_if_t<std::is_base_of_v<ENTRY, ENTRY_SUBTYPE>, std::nullptr_t> = nullptr>
        ENTRY_SUBTYPE* addEntry(std::unique_ptr<ENTRY_SUBTYPE> newEntry);

        /// Create a new entry and transfer it to the registry.
        template<typename ENTRY_SUBTYPE, typename... ARGS, std::enable_if_t<std::is_base_of_v<ENTRY, ENTRY_SUBTYPE>, std::nullptr_t> = nullptr>
        ENTRY_SUBTYPE* addEntry(ARGS&&... args) {
          return addEntry(std::make_unique<ENTRY_SUBTYPE>(std::forward<ARGS>(args)...));
        }

        /// Find an entry by an internal key which should be stable between
        /// versions of the program.
        /// If the provided identifier is unresolved, it will be resolved by setting its (mutable) discriminator to
        /// match that of the registered entry. Care should be taken to ensure the reference is not a temporary.
        const ENTRY* getEntryByIdentifier(const LongIdentifier& identifier) const;

        /// Find an entry which is expected to be present.
        /// Will throw an RegistryException if the entry is not found.
        /// If the provided identifier is unresolved, it will be resolved by setting its (mutable) discriminator to
        /// match that of the registered entry. Care should be taken to ensure the reference is not a temporary.
        const ENTRY& getRegisteredEntry(const LongIdentifier& identifier) const;

      public:
        class Iterator;
        // Iteration.
        Iterator begin() const;
        Iterator end() const;

      protected:
        UNTYPED_REGISTRY m_untypedRegistry;
    };

} // namespace babelwires

#include <Common/Registry/registry_inl.hpp>
