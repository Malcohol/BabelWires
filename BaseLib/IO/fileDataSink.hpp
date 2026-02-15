/**
 * A FileDataSink is a DataSink corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/IO/dataSink.hpp>
#include <BaseLib/Utilities/result.hpp>

#include <filesystem>
#include <fstream>

namespace babelwires {

    class FileDataSink : public DataSink {
      public:
        ~FileDataSink() override;

        /// Open a file for writing as a DataSink.
        static ResultT<FileDataSink> open(const std::filesystem::path& fileName);

        std::ostream& stream() override;

        FileDataSink(FileDataSink&&) = default;
        FileDataSink& operator=(FileDataSink&&) = default;

      private:
        Result doClose(ErrorState errorState) override;

        FileDataSink() = default;

        std::filesystem::path m_fileName;

        std::ofstream m_fileStream;
    };

} // namespace babelwires
