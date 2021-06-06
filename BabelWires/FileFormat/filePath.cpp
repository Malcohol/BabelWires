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

void babelwires::FilePath::resolveRelativeTo(const std::filesystem::path& base, UserLogger& userLogger) {
}

void babelwires::FilePath::interpretRelativeTo(const std::filesystem::path& base) {
}

std::string babelwires::FilePath::serializeToString() const {
    return m_filePath.u8string();
}

babelwires::FilePath babelwires::FilePath::deserializeFromString(const std::string& string) {
    std::filesystem::path path;
    try {
        path = string;
    }
    catch (const std::exception& e) {
        throw ParseException() << "Failed to parse \"" << string << "\" as a file path.";
    }
    return FilePath(path);
}
