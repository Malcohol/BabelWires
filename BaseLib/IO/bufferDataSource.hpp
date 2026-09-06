/**
 * A DataSource that serves bytes from an in-memory buffer.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/IO/dataSource.hpp>

namespace babelwires {
    /// A DataSource that serves bytes from an in-memory buffer.
    class BASELIB_API BufferDataSource : public DataSource {
      public:
        /// Serve the given bytes. The source is immediately at position 0 and
        /// EOF once the bytes are exhausted.
        explicit BufferDataSource(std::vector<Byte> bytes);

      protected:
        // Returns true.
        bool doIsEof() override;
        // Just asserts.
        ResultT<Byte> doGetNextByte() override;
    };
} // namespace babelwires
