/**
 * A FileDataSource is a DataSource corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/IO/fileDataSource.hpp>

#include <cassert>

babelwires::ResultT<babelwires::FileDataSource> babelwires::FileDataSource::open(const std::filesystem::path& fileName) {
    FileDataSource dataSource;
    dataSource.m_fileStream.open(fileName, std::ios_base::in | std::ios_base::binary);
    if (dataSource.m_fileStream.fail()) {
        return Error() << "Cannot open file " << fileName << " for reading";
    }
    return dataSource;
}

babelwires::Result babelwires::FileDataSource::close(ErrorState errorState) {
    if (m_fileStream.is_open()) {
        // Clear the error state so that close() will succeed even if we are at EOF or have had a read error.
        m_fileStream.clear();
        m_fileStream.close();
        if (m_fileStream.fail() && (errorState == ErrorState::NoError)) {
            return Error() << "Failed to close file";
        }
    }
    return {};
}

babelwires::FileDataSource::~FileDataSource() {
    assert(!m_fileStream.is_open() && "Close must be called on the FileDataSource before it is destroyed");
}

bool babelwires::FileDataSource::doIsEof() {
    return m_fileStream.peek() == std::istream::traits_type::eof();
}

babelwires::ResultT<babelwires::Byte> babelwires::FileDataSource::doGetNextByte() {
    assert(!doIsEof());
    int c = m_fileStream.get();
    if (m_fileStream.fail()) {
        return Error() << "Cannot read file";
    }
    return static_cast<Byte>(c);
}
