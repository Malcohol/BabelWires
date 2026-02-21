/**
 * A FileDataSource is a DataSource corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/IO/dataSource.hpp>
#include <BaseLib/Result/result.hpp>

#include <filesystem>
#include <fstream>

namespace babelwires {

    /// Either close or closeOnError must be called before the FileDataSource is destroyed.
    class FileDataSource : public DataSource {
      public:
        ~FileDataSource() override;

        /// Open a file for reading as a DataSource.
        static ResultT<FileDataSource> open(const std::filesystem::path& fileName);

        /// Close the file.
        Result close();
        /// Close the file when an error is being processed.
        void closeOnError();

        FileDataSource(FileDataSource&&) = default;
        FileDataSource& operator=(FileDataSource&&) = default;

      protected:
        virtual bool doIsEof() override;

        virtual ResultT<babelwires::Byte> doGetNextByte() override;

      private:
        FileDataSource() = default;

        // TODO Consider different back-end, e.g. FILE*.
        std::ifstream m_fileStream;
    };

} // namespace babelwires
