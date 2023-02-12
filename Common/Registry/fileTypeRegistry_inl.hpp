/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename ENTRY>
babelwires::FileTypeRegistry<ENTRY>::FileTypeRegistry(std::string registryName)
    : Registry<ENTRY, UntypedFileTypeRegistry>(std::move(registryName)) {}

template <typename ENTRY>
const ENTRY*
babelwires::FileTypeRegistry<ENTRY>::getEntryByFileName(std::string_view fileName) const {
    return static_cast<const ENTRY*>(FileTypeRegistry<ENTRY>::m_untypedRegistry.getEntryByFileName(fileName));
}

template <typename ENTRY>
babelwires::FileTypeEntry::Extensions babelwires::FileTypeRegistry<ENTRY>::getFileExtensions() const {
    return FileTypeRegistry<ENTRY>::m_untypedRegistry.getFileExtensions();
}

template <typename ENTRY>
void babelwires::FileTypeRegistry<ENTRY>::onEntryRegistered(ENTRY& newEntry) const {
    return FileTypeRegistry<ENTRY>::m_untypedRegistry.validateNewEntry(&newEntry);
}
