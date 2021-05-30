/**
 * A FileDataSource is a DataSource corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/IO/fileDataSource.hpp"
#include "Common/exceptions.hpp"

#include <cassert>

babelwires::FileDataSource::FileDataSource(const std::filesystem::path& fileName) {
    m_fileStream.open(fileName, std::ios_base::in | std::ios_base::binary);
    if (m_fileStream.fail()) {
        throw IoException() << "Cannot open file " << fileName << " for reading";
    }
}

bool babelwires::FileDataSource::doIsEof() {
    return m_fileStream.peek() == std::istream::traits_type::eof();
}

babelwires::Byte babelwires::FileDataSource::doGetNextByte() {
    assert(!doIsEof());
    int c = m_fileStream.get();
    if (m_fileStream.fail()) {
        throw IoException() << "Cannot read file";
    }
    return c;
}
