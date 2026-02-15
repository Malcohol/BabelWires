/**
 * A FileDataSink is a DataSink corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/IO/fileDataSink.hpp>

#include <cassert>

babelwires::ResultT<babelwires::FileDataSink>
babelwires::FileDataSink::open(const std::filesystem::path& fileName) {
    FileDataSink dataSink;
    dataSink.m_fileName = fileName;
    dataSink.m_fileStream.open(fileName, std::ios_base::out | std::ios_base::binary);
    if (dataSink.m_fileStream.fail()) {
        dataSink.close(ErrorState::Error);
        return Error() << "Cannot open file " << fileName << " for writing";
    }
    return dataSink;
}

babelwires::Result babelwires::FileDataSink::doClose(ErrorState errorState) {
    if (m_fileStream.is_open()) {
        const bool hadWriteFailure = m_fileStream.fail();
        m_fileStream.clear();
        m_fileStream.close();
        if ((errorState == ErrorState::NoError) && hadWriteFailure) {
            return Error() << "Failed to write to file " << m_fileName;
        }
        if (m_fileStream.fail() && (errorState == ErrorState::NoError)) {
            return Error() << "Failed to close file " << m_fileName;
        }
    }
    return {};
}

std::ostream& babelwires::FileDataSink::stream() {
    return m_fileStream;
}

babelwires::FileDataSink::~FileDataSink() {
    assert(!m_fileStream.is_open() && "Close must be called on the FileDataSink before it is destroyed");
}
