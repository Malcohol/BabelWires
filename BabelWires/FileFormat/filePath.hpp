/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <filesystem>

namespace babelwires {
    class UserLogger;

    /// How file locations are represented in project data.
    class FilePath {
      public:
        FilePath() = default;
        FilePath(const FilePath&) = default;
        FilePath(FilePath&&) = default;

        FilePath& operator=(const FilePath&) = default;
        FilePath& operator=(FilePath&&) = default;

        FilePath(std::filesystem::path absolutePath);
        FilePath& operator=(std::filesystem::path absolutePath);
        operator std::filesystem::path() const;

        inline friend bool operator==(const FilePath& a, const FilePath& b) { 
            return a.m_filePath == b.m_filePath;
        }

        inline friend bool operator==(const std::filesystem::path& a, const FilePath& b) { 
            return a == b.m_filePath;
        }

        inline friend bool operator==(const FilePath& a, const std::filesystem::path& b) { 
            return a.m_filePath == b;
        }

        bool empty() const;

        /// Interpret relative paths with respect to the given base path and, if there is a file at that location, set the absolute path
        /// to that absolute location. Otherwise, leave absolute paths unchanged.
        void resolveRelativeTo(const std::filesystem::path& base, UserLogger& userLogger);

        /// Set the relative paths from the absolute paths, starting at the given base path.
        void interpretRelativeTo(const std::filesystem::path& base);

        /// Get a serializable representation of the path.
        std::string serializeToString() const;
        
        /// Parse a serialized representation of a path.
        static FilePath deserializeFromString(const std::string& string);

      private:
        /// In the running system, this is the absolute path of the file.
        std::filesystem::path m_filePath;
    };
} // namespace babelwires
