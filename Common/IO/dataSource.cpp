/**
 * A DataSource provides a source of bytes with a simple mechanism for rewinding.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/IO/dataSource.hpp"
#include "Common/exceptions.hpp"

#include <cassert>

babelwires::DataSource::DataSource()
    : m_positionOfCursorOrBuffer(-1)
    , m_indexInBuffer(0) {}

babelwires::DataSource::~DataSource() {}

int babelwires::DataSource::getRemainingBufferSize() const {
    return m_buffer.size() - m_indexInBuffer;
}

bool babelwires::DataSource::isEof() {
    return doIsEof();
}

babelwires::Byte babelwires::DataSource::getNextByte() {
    if (getRemainingBufferSize() > 0) {
        babelwires::Byte b = m_buffer[m_indexInBuffer];
        ++m_indexInBuffer;
        return b;
    } else if (doIsEof()) {
        throw IoException() << "End of file reached when trying to get byte " << m_positionOfCursorOrBuffer
                            << " from data source";
    } else {
        if (m_buffer.size()) {
            m_positionOfCursorOrBuffer += m_indexInBuffer;
            m_indexInBuffer = 0;
            m_buffer.clear();
        }
        ++m_positionOfCursorOrBuffer;
        return doGetNextByte();
    }
}

babelwires::Byte babelwires::DataSource::peekNextByte() {
    if (getRemainingBufferSize() == 0) {
        setRewindPoint(1);
        return m_buffer[0];
    } else {
        return m_buffer[m_indexInBuffer];
    }
}

int babelwires::DataSource::setRewindPoint(int numBytes) {
    m_buffer.reserve(numBytes);
    int numExtraBytesToRead = numBytes;
    const int remainingBufferSize = getRemainingBufferSize();
    if (remainingBufferSize > 0) {
        for (int i = 0; i < remainingBufferSize; ++i) {
            m_buffer[i] = m_buffer[m_indexInBuffer + i];
        }
        m_buffer.resize(remainingBufferSize);
        numExtraBytesToRead = (numBytes > remainingBufferSize) ? (numBytes - remainingBufferSize) : 0;
    } else {
        m_buffer.clear();
    }
    int numBytesRead = remainingBufferSize;
    for (int i = 0; i < numExtraBytesToRead; ++i) {
        if (doIsEof()) {
            break;
        }
        ++numBytesRead;
        m_buffer.push_back(doGetNextByte());
    }
    m_positionOfCursorOrBuffer += m_indexInBuffer;
    m_indexInBuffer = 0;
    return std::min(numBytesRead, numBytes);
}

void babelwires::DataSource::rewind() {
    assert(m_buffer.size() > 0);
    m_indexInBuffer = 0;
}

int babelwires::DataSource::getAbsolutePosition() const {
    return m_positionOfCursorOrBuffer + m_indexInBuffer;
}
