/**
 * A FileDataSource is a DataSource corresponding to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/IO/dataSource.hpp"

#include <fstream>

namespace babelwires {

    class FileDataSource : public DataSource {
      public:
        FileDataSource(const char* fileName);

      protected:
        virtual bool doIsEof() override;

        virtual babelwires::Byte doGetNextByte() override;

      private:
        // TODO Consider different back-end, e.g. FILE*.
        std::ifstream m_fileStream;
    };

} // namespace babelwires
