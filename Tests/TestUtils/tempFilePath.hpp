#pragma once

#include <filesystem>
#include <string_view>

namespace testUtils {
    struct TempFilePath {
        /// Generate a path to a temp file with the given file name.
        /// Attempts to delete any file which is already at that path.
        TempFilePath(std::string_view fileName);

        TempFilePath(TempFilePath&& other);

        /// Attempts to clean up the path, if it exists.
        ~TempFilePath();

        /// Attempts to clean up the path, if it exists.
        void tryRemoveFile();

        operator const std::filesystem::path &();

        operator const char*();

        std::filesystem::path m_filePath;
    };
} // namespace testUtils
