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
        /// The absolute path is returned.
        operator std::filesystem::path() const;

        /// Interpret relative paths with respect to the given base path and, if there is a file at that location, set the absolute path
        /// to that absolute location. Otherwise, leave absolute paths unchanged.
        void resolveRelativeTo(const std::filesystem::path& base, UserLogger& log);

        /// Set the relative paths from the absolute paths, starting at the given base path.
        void interpretRelativeTo(const std::filesystem::path& base);

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
