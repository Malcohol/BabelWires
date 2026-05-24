/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/DataContext/filePath.hpp>

#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/common.hpp>
#include <BaseLib/Result/resultDSL.hpp>

#include <cassert>
#include <cctype>

babelwires::FilePath::FilePath(std::filesystem::path absolutePath)
    : m_filePath(std::move(absolutePath)) {}

babelwires::FilePath& babelwires::FilePath::operator=(std::filesystem::path absolutePath) {
    m_filePath.swap(absolutePath);
    return *this;
}

babelwires::FilePath::operator std::filesystem::path() const {
    return m_filePath;
}

bool babelwires::FilePath::empty() const {
    return m_filePath.empty();
}

void babelwires::FilePath::resolveRelativeTo(const std::filesystem::path& newBase, const std::filesystem::path& oldBase, UserLogger& userLogger) {
    const auto isAbsoluteOrRooted = [](const std::filesystem::path& path) {
        return path.is_absolute() || path.has_root_directory();
    };

    const auto preferResolvedPath = [&userLogger](const std::filesystem::path& newPath,
                                                  const std::filesystem::path& oldPath) -> std::filesystem::path {
        // Prefer new relative file locations to old ones.
        if (std::filesystem::exists(newPath)) {
            std::filesystem::path resolvedPath = std::filesystem::canonical(newPath);
            if (std::filesystem::exists(oldPath)) {
                const std::filesystem::path oldCanonical = std::filesystem::canonical(oldPath);
                if (resolvedPath != oldCanonical) {
                    // Info, not warning, because this should be the expected behaviour.
                    userLogger.logInfo()
                        << "Favouring file " << resolvedPath << " over file " << oldCanonical
                        << ", because its location relative to the project is preserved";
                }
            }
            return resolvedPath;
        }
        // We presume the old path provides more context.
        return oldPath;
    };

    // Note: A Windows-style absolute path might appear relative to a posix platform.
    // Since project paths are intended to be absolute, we rely on the fact that they will appear relative too
    // to avoid building a nonsensical path like c:/path/to/project/d:/path/to/file.
    if (!isAbsoluteOrRooted(m_filePath) && (oldBase.empty() || isAbsoluteOrRooted(oldBase))) {
        m_filePath = preferResolvedPath(newBase / m_filePath, oldBase / m_filePath);
        return;
    }

    if (isAbsoluteOrRooted(m_filePath) && !newBase.empty() && !oldBase.empty() && isAbsoluteOrRooted(oldBase)) {
        const std::filesystem::path relativePath = m_filePath.lexically_relative(oldBase);
        if (!relativePath.empty() && !isAbsoluteOrRooted(relativePath)) {
            m_filePath = preferResolvedPath(newBase / relativePath, m_filePath);
        }
    }
}

void babelwires::FilePath::interpretRelativeTo(const std::filesystem::path& base) {
    std::filesystem::path relPath = std::filesystem::relative(m_filePath, base);
    if (!relPath.empty()) {
        m_filePath = std::move(relPath);
    }
}

std::string babelwires::FilePath::serializeToString() const {
    return pathToString(m_filePath);
}

babelwires::ResultT<babelwires::FilePath> babelwires::FilePath::deserializeFromString(const std::string& string) {
    // TODO - validate the path string, and return an error if it is invalid.
    // u8path only performs string construction and does not validate the path,
    return FilePath(std::filesystem::u8path(string));
}
