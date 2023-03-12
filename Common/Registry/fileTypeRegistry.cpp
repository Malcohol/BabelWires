/**
 * The FileTypeRegistry is a registry that can be queried by file extension.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/Registry/fileTypeRegistry.hpp>

#include <cctype>

babelwires::FileTypeEntry::FileTypeEntry(LongIdentifier identifier, VersionNumber version,
                                         Extensions extensions)
    : RegistryEntry(identifier, version)
    , m_extensions(std::move(extensions)) {
    for (auto& x : m_extensions) {
        assert(!x.empty());
        std::for_each(x.begin(), x.end(), [](auto& c) { c = std::tolower(c); });
    }
}
