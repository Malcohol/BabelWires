/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/FileFormat/filePath.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

#include "Common/Log/userLogger.hpp"

#include <cassert>

babelwires::FilePath::operator std::filesystem::path() const {
    return m_absolutePath;
}

void babelwires::FilePath::resolveRelativeTo(const std::filesystem::path& base, UserLogger& log) {
    assert(base.is_absolute() && "The base path must be absolute");
    if (!m_relativePath.empty()) {
        std::filesystem::path absPath = base / m_relativePath;
        if (std::filesystem::exists(absPath)) {
            if (std::filesystem::exists(m_absolutePath)) {
                if (std::filesystem::canonical(m_absolutePath) != std::filesystem::canonical(absPath)) {
                    log.logWarning() << "Favouring file " << absPath << " over file " << m_absolutePath
                                     << ", as its location relative to the project is maintained";
                }
            }
            m_absolutePath = std::move(absPath);
        }
    }
}

void babelwires::FilePath::interpretRelativeTo(const std::filesystem::path& base) {
    assert(base.is_absolute() && "The base path must be absolute");
    std::filesystem::path relPath = std::filesystem::relative(m_absolutePath, base);
    if (!relPath.empty()) {
        m_relativePath = std::move(relPath);
    }
}

void babelwires::FilePath::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("absolutePath", m_absolutePath.u8string());
    serializer.serializeValue("relativePath", m_relativePath.u8string());
}

void babelwires::FilePath::deserializeContents(Deserializer& deserializer) {
    {
        std::string absPath;
        if (deserializer.deserializeValue("absolutePath", absPath, Deserializer::IsOptional::Optional)) {
            m_absolutePath = absPath;
        }
    }
    {
        std::string relPath;
        if (deserializer.deserializeValue("relativePath", relPath, Deserializer::IsOptional::Optional)) {
            m_relativePath = relPath;
        }
    }
    if (m_relativePath.empty() && m_absolutePath.empty()) {
        throw ParseException() << "A filePath object must have a relative or absolute path set";
    }
}
