#pragma once

#include <filesystem>
#include <string_view>

namespace testUtils {
    class TempFilePath {
      public:
        /// Generate a path to a temp file with the given file name.
        /// Attempts to delete any file which is already at that path.
        TempFilePath(std::string_view fileName);

        TempFilePath(TempFilePath&& other);

        /// Attempts to clean up the path, if it exists.
        ~TempFilePath();

        /// Attempts to clean up the path, if it exists.
        void tryRemoveFile();

        /// Create the file if it doesn't exists and write the given contents.
        void ensureExists(std::string contents = "TestContents");

        operator const std::filesystem::path&();

        operator const char*();

        std::filesystem::path m_filePath;
        std::string m_asString;
    };

    class TempDirectory {
      public:
        TempDirectory(std::string_view dirPath);

        ~TempDirectory();

      private:
        std::filesystem::path m_dirPath;
    };
} // namespace testUtils
