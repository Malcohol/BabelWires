/**
 * A FileDataSource is a DataSource corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/IO/dataSource.hpp>
#include <BaseLib/Utilities/result.hpp>

#include <fstream>
#include <filesystem>

namespace babelwires {

    class FileDataSource : public DataSource {
      public:
        /// Open a file for reading as a DataSource.
        static ResultT<FileDataSource> open(const std::filesystem::path& fileName);

        /// Close the file. The errorState parameter allows the close to be combined with
        /// FINALLY_WITH_ERRORSTATE to ensure the file gets closed in the error case.
        Result close(ErrorState errorState = ErrorState::NoError);

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
