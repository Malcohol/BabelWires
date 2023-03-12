/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Registry/registry.hpp>

namespace babelwires {

    /// A registry entry which corresponds to some file extensions.
    class FileTypeEntry : public RegistryEntry {
      public:
        using Extensions = std::vector<std::string>;

        /// Extensions are stored in lower case, regardless of what is passed in.
        /// Extensions should not include the ".".
        FileTypeEntry(LongId identifier, VersionNumber version, Extensions extensions);

        const Extensions& getFileExtensions() const { return m_extensions; }

      private:
        Extensions m_extensions;
    };

    /// A registry whose entries can be queried based on file extensions.
    template <typename ENTRY> class FileTypeRegistry : public Registry<ENTRY> {
      public:
        FileTypeRegistry(std::string registryName);

        /// Search for an entry by looking for a matching extension.
        /// A fileName which is just the extension will also match.
        const ENTRY* getEntryByFileName(std::string_view fileName) const;

        /// Get the file extensions of all registered extensions in no particular order.
        FileTypeEntry::Extensions getFileExtensions() const;

        void onEntryRegistered(ENTRY& newEntry) const override;

      private:
        const ENTRY* getEntryByExtension(std::string extension) const;
    };
} // namespace babelwires

#include <Common/Registry/fileTypeRegistry_inl.hpp>