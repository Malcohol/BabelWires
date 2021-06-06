/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/FileFormat/filePath.hpp"

#include "Common/Log/userLogger.hpp"
#include "Common/exceptions.hpp"

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
    if (!m_filePath.is_absolute()) {
        std::filesystem::path newPath = newBase / m_filePath;
        std::filesystem::path oldPath = oldBase / m_filePath;
        // Prefer new relative file locations to old ones.
        if (std::filesystem::exists(newPath)) {
            m_filePath = std::filesystem::canonical(newPath);
            if (std::filesystem::exists(oldPath)) {
                const std::filesystem::path oldCanonical = std::filesystem::canonical(oldPath);
                if (m_filePath != oldPath) {
                    userLogger.logWarning()
                        << "Favouring file " << m_filePath << " over file " << oldCanonical
                        << ", as its location relative to the project is maintained";
                }
            }
        } else {
            // We presume the old path provides more context.
            m_filePath = std::move(oldPath);
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
    return m_filePath.u8string();
}

babelwires::FilePath babelwires::FilePath::deserializeFromString(const std::string& string) {
    if (std::all_of(string.begin(),string.end(),::isspace)) {
        throw ParseException() << "Empty file path";
    }
    std::filesystem::path path;
    try {
        path = string;
    }
    catch (const std::exception& e) {
        throw ParseException() << "Failed to parse \"" << string << "\" as a file path.";
    }
    return FilePath(path);
}
