/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Registry/registry.hpp"

namespace babelwires {

    /// A registry entry which corresponds to some file extensions.
    class FileTypeEntry : public RegistryEntry {
      public:
        using Extensions = std::vector<std::string>;

        /// Extensions are stored in lower case, regardless of what is passed in.
        /// Extensions should not include the ".".
        FileTypeEntry(LongIdentifier identifier, VersionNumber version, Extensions extensions);

        const Extensions& getFileExtensions() const { return m_extensions; }

      private:
        Extensions m_extensions;
    };

    /// Adds file extension logic to the untyped registry.
    class UntypedFileTypeRegistry : public UntypedRegistry {
      public:
        UntypedFileTypeRegistry(std::string registryName);

        /// Get the file extensions of all registered extensions in no particular order.
        FileTypeEntry::Extensions getFileExtensions() const;

        /// Search for an entry by looking for a matching extension.
        /// A fileName which is just the extension will also match.
        const RegistryEntry* getEntryByFileName(std::string_view fileName) const;

      protected:
        virtual void validateNewEntry(const RegistryEntry* newEntry) const override;

        const RegistryEntry* getEntryByExtension(std::string extension) const;
    };

    /// A registry whose entries can be queried based on file extensions.
    template <typename ENTRY, typename UNTYPED_REGISTRY = UntypedFileTypeRegistry>
    class FileTypeRegistry : public Registry<ENTRY, UNTYPED_REGISTRY> {
      public:
        FileTypeRegistry(std::string registryName);

        /// Search for an entry by looking for a matching extension.
        /// A fileName which is just the extension will also match.
        const ENTRY* getEntryByFileName(std::string_view fileName) const;

        /// Get the file extensions of all registered extensions in no particular order.
        FileTypeEntry::Extensions getFileExtensions() const;
    };

} // namespace babelwires

#include "Common/Registry/fileTypeRegistry_inl.hpp"