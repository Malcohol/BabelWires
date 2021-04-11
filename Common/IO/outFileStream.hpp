/**
 * An OutFileStream provides an exception throwing ofstream.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <filesystem>
#include <fstream>

namespace babelwires {
    /// Provides an exception throwing ofstream. Adds the requirement that normal
    /// code-paths must call close(), in order to provide a site for
    /// exception handling.
    // TODO: Replace uses of ostream with something capable of throwing more detailed exceptions.
    class OutFileStream : public std::ofstream {
      public:
        OutFileStream(const std::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::out);

        /// This asserts that the stream has been already been closed.
        ~OutFileStream();
    };
} // namespace babelwires
