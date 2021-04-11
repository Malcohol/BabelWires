#include "Tests/TestUtils/tempFilePath.hpp"

testUtils::TempFilePath::TempFilePath(std::string_view fileName)
    : m_filePath(std::filesystem::temp_directory_path() / fileName) {
    tryRemoveFile();
}

testUtils::TempFilePath::TempFilePath(TempFilePath&& other) {
    m_filePath = other.m_filePath;
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

testUtils::TempFilePath::operator const std::filesystem::path &() {
    return m_filePath;
}

testUtils::TempFilePath::operator const char*() {
    return m_filePath.c_str();
}
