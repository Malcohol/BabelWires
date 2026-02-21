/**
 * A DataSink provides a destination for bytes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/common.hpp>
#include <BaseLib/Utilities/result.hpp>

#include <cassert>
#include <ostream>

namespace babelwires {

    /// Provide a destination for bytes.
    class DataSink {
      public:
        DataSink() = default;
        DataSink(DataSink&& other) noexcept;
        DataSink& operator=(DataSink&& other) noexcept;
        DataSink(const DataSink&) = delete;
        DataSink& operator=(const DataSink&) = delete;

        virtual ~DataSink();

        /// Access the stream used for writing.
        virtual std::ostream& stream() = 0;

        /// Close the sink.
        Result close();
        /// Close the sink when an error is being processed.
        void closeOnError();

      protected:
        virtual Result doClose() = 0;
        virtual void doCloseOnError() = 0;

      private:
        bool m_hasBeenClosed = false;
    };

} // namespace babelwires
