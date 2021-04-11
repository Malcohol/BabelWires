/**
 * An OutFileStream provides an exception throwing ofstream.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/IO/outFileStream.hpp"

#include <cassert>

babelwires::OutFileStream::OutFileStream(const std::filesystem::path& path, std::ios_base::openmode mode) {
    exceptions(std::ios_base::failbit | std::ios_base::badbit);
    open(path, mode);
}

babelwires::OutFileStream::~OutFileStream() {
    // Enforce the requirement that the stream be closed before destruction, so
    // exceptions can be handled.
    assert((!is_open() || (std::uncaught_exceptions() > 0)) && "The stream was not closed before destruction");
    // Investigate dropping any unflushed contents, to force non-debug builds to call close().
}
