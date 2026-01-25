/**
 * A FilePath is how file locations are represented in project data.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/result.hpp>

#include <filesystem>
#include <ostream>

namespace babelwires {
    struct UserLogger;

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

        /// Is the file path just the empty path?
        bool empty() const;

        /// Interpret m_filePath as relative to newBase if it exists. Otherwise, interpret it as relative to oldBase.
        void resolveRelativeTo(const std::filesystem::path& newBase, const std::filesystem::path& oldBase, UserLogger& userLogger);

        /// Set the relative paths from the absolute paths, starting at the given base path.
        void interpretRelativeTo(const std::filesystem::path& base);

        /// Get a serializable representation of the path.
        std::string serializeToString() const;
        
        /// Parse a serialized representation of a path.
        static ParseResult<FilePath> deserializeFromString(const std::string& string);

      public:
        /// Used when a test fails to print the paths.
        inline friend std::ostream& operator<<(std::ostream& os, const FilePath& p) {
            return os << p.m_filePath;
        }

      private:
        /// In the running system, this is the absolute path of the file.
        std::filesystem::path m_filePath;
    };
} // namespace babelwires
