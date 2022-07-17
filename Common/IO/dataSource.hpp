/**
 * A DataSource provides a source of bytes with a simple mechanism for rewinding.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/types.hpp>

namespace babelwires {

    /// Provide a source of bytes with a simple mechanism for rewinding.
    class DataSource {
      public:
        DataSource();

        virtual ~DataSource();

        /// Consume a byte.
        /// Throws if there are no more bytes.
        Byte getNextByte();

        /// Peek at the next byte without consuming it.
        /// Throws if there are no more bytes.
        Byte peekNextByte();

        /// Return the position, within the whole source, of the last byte read.
        int getAbsolutePosition() const;

        /// Checks whether every byte has been read.
        bool isEof();

        /// Set rewind point.
        /// If you set a new rewind point, read or peek more than numBytes, the rewind point is discarded.
        int setRewindPoint(int numBytes);

        /// Reset the stream to the last rewind point.
        void rewind();

      protected:
        virtual bool doIsEof() = 0;

        virtual babelwires::Byte doGetNextByte() = 0;

      private:
        int getRemainingBufferSize() const;

      private:
        /// Bytes are stashed here.
        std::vector<Byte> m_buffer;

        int m_indexInBuffer;

        /// The position of the last byte read.
        int m_positionOfCursorOrBuffer;
    };

} // namespace babelwires
