#include "Tests/TestUtils/tempFilePath.hpp"

#include <fstream>
#include <cassert>

testUtils::TempFilePath::TempFilePath(std::string_view fileName)
    : m_filePath(std::filesystem::temp_directory_path() / fileName)
    , m_asString(m_filePath.u8string()) {
    tryRemoveFile();
}

testUtils::TempFilePath::TempFilePath(TempFilePath&& other) {
    m_filePath = std::move(other.m_filePath);
    m_asString = std::move(other.m_asString);
    other.m_filePath.clear();
}

testUtils::TempFilePath::~TempFilePath() {
    tryRemoveFile();
}

void testUtils::TempFilePath::tryRemoveFile() {
    if (!m_filePath.empty()) {
        std::filesystem::remove(m_filePath);
    }
}

testUtils::TempFilePath::operator const std::filesystem::path&() {
    return m_filePath;
}

testUtils::TempFilePath::operator const char*() {
    return m_asString.c_str();
}

void testUtils::TempFilePath::ensureExists(std::string contents) {
    std::ofstream fs(m_filePath, std::ofstream::out);
    fs << contents;
    fs.close();
    assert(fs.good() && "Unable to create test file");
}

testUtils::TempDirectory::TempDirectory(std::string_view dirPath)
    : m_dirPath(std::filesystem::temp_directory_path() / dirPath) {
    std::filesystem::create_directories(m_dirPath);
}

testUtils::TempDirectory::~TempDirectory() {
    const std::string tmpString = std::filesystem::temp_directory_path().u8string();

    const auto isUnderTemp = [&tmpString](const std::filesystem::path& p) {
        const std::string pString = p.u8string();
        return (pString.find(tmpString) == 0);
    };

    std::filesystem::path p = m_dirPath;

    // Being very careful here.
    assert(isUnderTemp(p) && "Attempted to delete directories which were not in the temp directory");

    while (isUnderTemp(p)) {
        // remove should only work on empty directories, but let's be extra careful.
        if (std::filesystem::is_directory(p) && std::filesystem::is_empty(p)) {
            std::filesystem::remove(p);
        }
        p = p.parent_path();
    }
}
