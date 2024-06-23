#include <Tests/TestUtils/tempFilePath.hpp>

#include <Common/types.hpp>

#include <cassert>
#include <fstream>
#include <string>

namespace {
    std::filesystem::path discriminateFileName(std::string_view fileName, std::string discriminator) {
        std::filesystem::path filePath = fileName;
        if (!discriminator.empty()) {
            const std::filesystem::path extension = filePath.extension();
            filePath.replace_extension("");
            filePath += "_" + discriminator;
            filePath.replace_extension(extension);
        }
        return filePath;
    }

    std::filesystem::path discriminateFileName(std::string_view fileName, int discriminator) {
        return discriminateFileName(fileName, discriminator ? std::to_string(discriminator) : std::string(""));
    }
} // namespace

testUtils::TempFilePath::TempFilePath(std::string_view fileName, int discriminator)
    : m_filePath(std::filesystem::canonical(std::filesystem::temp_directory_path()) /
                 discriminateFileName(fileName, discriminator))
    , m_asString(babelwires::from_u8string(m_filePath.u8string())) {
    tryRemoveFile();
}

testUtils::TempFilePath::TempFilePath(std::string_view fileName, std::string_view discriminator)
    : m_filePath(std::filesystem::canonical(std::filesystem::temp_directory_path()) /
                 discriminateFileName(fileName, std::string(discriminator)))
    , m_asString(babelwires::from_u8string(m_filePath.u8string())) {
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

std::ofstream testUtils::TempFilePath::openForWriting(std::ios_base::openmode mode) const {
    return std::ofstream(m_asString, mode);
}

std::ifstream testUtils::TempFilePath::openForReading(std::ios_base::openmode mode) const {
    return std::ifstream(m_asString, mode);
}

void testUtils::TempFilePath::ensureExists(std::string contents) {
    std::ofstream fs(m_filePath, std::ofstream::out);
    fs << contents;
    fs.close();
    assert(fs.good() && "Unable to create test file");
}

testUtils::TempDirectory::TempDirectory(std::string_view dirPath)
    : m_dirPath(std::filesystem::canonical(std::filesystem::temp_directory_path()) / dirPath) {
    std::filesystem::create_directories(m_dirPath);
}

testUtils::TempDirectory::~TempDirectory() {
    const std::string tmpString = babelwires::from_u8string(std::filesystem::canonical(std::filesystem::temp_directory_path()).u8string());

    const auto isUnderTemp = [&tmpString](const std::filesystem::path& p) {
        const std::string pString = babelwires::from_u8string(p.u8string());
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
