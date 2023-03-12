/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename ENTRY>
babelwires::FileTypeRegistry<ENTRY>::FileTypeRegistry(std::string registryName)
    : Registry<ENTRY>(std::move(registryName)) {}

template <typename ENTRY>
const ENTRY*
babelwires::FileTypeRegistry<ENTRY>::getEntryByFileName(std::string_view fileName) const {
    assert(!fileName.empty());
    const std::size_t dotIndex = fileName.rfind('.');
    const std::size_t start = (dotIndex != std::string::npos) ? (dotIndex + 1) : 0;
    return getEntryByExtension(std::string(fileName.substr(start)));
}

template <typename ENTRY>
babelwires::FileTypeEntry::Extensions babelwires::FileTypeRegistry<ENTRY>::getFileExtensions() const {
    babelwires::FileTypeEntry::Extensions extensions;
    for (const auto& entry : *this) {
        const auto& theseExtensions = entry.getFileExtensions();
        extensions.insert(extensions.end(), theseExtensions.begin(), theseExtensions.end());
    }
    return extensions;
}

template <typename ENTRY>
const ENTRY* babelwires::FileTypeRegistry<ENTRY>::getEntryByExtension(std::string extension) const {
    assert(!extension.empty());
    std::for_each(extension.begin(), extension.end(), [](auto& c) { c = std::tolower(c); });

    for (const auto& entry : *this) {
        const auto& theseExtensions = entry.getFileExtensions();
        if (std::find(theseExtensions.begin(), theseExtensions.end(), extension) != theseExtensions.end()) {
            return &entry;
        }
    }
    return nullptr;
}

template <typename ENTRY>
void babelwires::FileTypeRegistry<ENTRY>::onEntryRegistered(ENTRY& newEntry) const {
    const auto& currentExtensions = getFileExtensions();

    for (const auto& n : newEntry.getFileExtensions()) {
        assert((n.rfind('.') == std::string::npos) && "extensions may not contain '.'");
        assert((std::find(currentExtensions.begin(), currentExtensions.end(), n) == currentExtensions.end()) &&
               "Extension already handled by a format");
    }
}
