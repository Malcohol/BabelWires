/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename ENTRY, typename UNTYPED_REGISTRY>
babelwires::FileTypeRegistry<ENTRY, UNTYPED_REGISTRY>::FileTypeRegistry(std::string registryName)
    : Registry<ENTRY, UntypedFileTypeRegistry>(std::move(registryName)) {}

template <typename ENTRY, typename UNTYPED_REGISTRY>
const ENTRY*
babelwires::FileTypeRegistry<ENTRY, UNTYPED_REGISTRY>::getEntryByFileName(std::string_view fileName) const {
    return static_cast<const ENTRY*>(Registry<ENTRY, UNTYPED_REGISTRY>::m_untypedRegistry.getEntryByFileName(fileName));
}

template <typename ENTRY, typename UNTYPED_REGISTRY>
babelwires::FileTypeEntry::Extensions babelwires::FileTypeRegistry<ENTRY, UNTYPED_REGISTRY>::getFileExtensions() const {
    return Registry<ENTRY, UNTYPED_REGISTRY>::m_untypedRegistry.getFileExtensions();
}
