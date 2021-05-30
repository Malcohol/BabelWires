/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Serialization/serializable.hpp"

#include <filesystem>

namespace babelwires {
    class UserLogger;

    /// How file locations are represented in project data.
    class FilePath : public Serializable {
      public:
        SERIALIZABLE(FilePath, "file", void, 1);
        FilePath() = default;
        FilePath(const FilePath&) = default;
        FilePath(FilePath&&) = default;

        FilePath& operator=(const FilePath&) = default;
        FilePath& operator=(FilePath&&) = default;

        FilePath(std::filesystem::path absolutePath);
        FilePath& operator=(std::filesystem::path absolutePath);
        operator std::filesystem::path() const;

        inline friend bool operator==(const FilePath& a, const FilePath& b) { 
            return a.m_absolutePath == b.m_absolutePath;
        }

        inline friend bool operator==(const std::filesystem::path& a, const FilePath& b) { 
            return a == b.m_absolutePath;
        }

        inline friend bool operator==(const FilePath& a, const std::filesystem::path& b) { 
            return a.m_absolutePath == b;
        }

        bool empty() const;

        /// Interpret relative paths with respect to the given base path and, if there is a file at that location, set the absolute path
        /// to that absolute location. Otherwise, leave absolute paths unchanged.
        void resolveRelativeTo(const std::filesystem::path& base, UserLogger& userLogger);

        /// Set the relative paths from the absolute paths, starting at the given base path.
        void interpretRelativeTo(const std::filesystem::path& base);

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      private:
        /// Used by the running system, this uniquely defines the file location.
        std::filesystem::path m_absolutePath;

        /// Used in addition to the absolute path when project data is in serialized form.
        /// When data is loaded into a project, we attempt to resolve files first by their project-relative location,
        /// and fall back to absolute paths only if that fails.
        /// This allows a project and its imports to be relocated.
        std::filesystem::path m_relativePath;
    };
} // namespace babelwires
