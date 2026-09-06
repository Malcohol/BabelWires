/**
 * A DataSource that serves bytes from an in-memory buffer.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BaseLib/IO/bufferDataSource.hpp>

#include <BaseLib/Result/error.hpp>

#include <cassert>

babelwires::BufferDataSource::BufferDataSource(std::vector<Byte> bytes)
    : DataSource(std::move(bytes)) {}

bool babelwires::BufferDataSource::doIsEof() {
    return true; // all data lives in the buffer
}

babelwires::ResultT<babelwires::Byte> babelwires::BufferDataSource::doGetNextByte() {
    assert(false && "doGetNextByte should never be called on BufferDataSource");
    return Error();
}
