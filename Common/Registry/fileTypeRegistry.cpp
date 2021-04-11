#include "Common/Registry/fileTypeRegistry.hpp"

#include <cctype>

babelwires::FileTypeEntry::FileTypeEntry(std::string identifier, std::string name, VersionNumber version,
                                         Extensions extensions)
    : RegistryEntry(std::move(identifier), std::move(name), version)
    , m_extensions(std::move(extensions)) {
    for (auto& x : m_extensions) {
        assert(!x.empty());
        std::for_each(x.begin(), x.end(), [](auto& c) { c = std::tolower(c); });
    }
}

babelwires::UntypedFileTypeRegistry::UntypedFileTypeRegistry(std::string registryName)
    : UntypedRegistry(std::move(registryName)) {}

void babelwires::UntypedFileTypeRegistry::validateNewEntry(const RegistryEntry* newEntry) const {
    // Super-call.
    UntypedRegistry::validateNewEntry(newEntry);

    const auto& currentExtensions = getFileExtensions();

    for (const auto& n : static_cast<const FileTypeEntry*>(newEntry)->getFileExtensions()) {
        assert((n.rfind('.') == std::string::npos) && "extensions may not contain '.'");
        assert((std::find(currentExtensions.begin(), currentExtensions.end(), n) == currentExtensions.end()) &&
               "Extension already handled by a format");
    }
}

babelwires::FileTypeEntry::Extensions babelwires::UntypedFileTypeRegistry::getFileExtensions() const {
    babelwires::FileTypeEntry::Extensions extensions;
    for (const auto& f : *this) {
        const auto& theseExtensions = static_cast<const FileTypeEntry*>(f.get())->getFileExtensions();
        extensions.insert(extensions.end(), theseExtensions.begin(), theseExtensions.end());
    }
    return extensions;
}

const babelwires::RegistryEntry* babelwires::UntypedFileTypeRegistry::getEntryByExtension(std::string extension) const {
    assert(!extension.empty());
    std::for_each(extension.begin(), extension.end(), [](auto& c) { c = std::tolower(c); });

    for (const auto& f : *this) {
        const auto& theseExtensions = static_cast<const FileTypeEntry*>(f.get())->getFileExtensions();
        if (std::find(theseExtensions.begin(), theseExtensions.end(), extension) != theseExtensions.end()) {
            return &*f;
        }
    }
    return nullptr;
}

const babelwires::RegistryEntry*
babelwires::UntypedFileTypeRegistry::getEntryByFileName(std::string_view fileName) const {
    assert(!fileName.empty());
    const std::size_t dotIndex = fileName.rfind('.');
    const std::size_t start = (dotIndex != std::string::npos) ? (dotIndex + 1) : 0;
    return getEntryByExtension(std::string(fileName.substr(start)));
}
