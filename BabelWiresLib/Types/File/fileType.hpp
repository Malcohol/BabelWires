/**
 * A type which corresponds to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    class FileType : public RecordType {
      public:
        FileType(ShortId stepToContents, TypeRef typeOfContents);

        std::string getKind() const override;

      protected:
        /// Available for FailedFileType.
        FileType();
    };

    /// This Type can be used when a file fails to load.
    // TODO Would a default constructed subclass of the format's type be better?
    class FailedFileType : public FileType {
      public:
        PRIMITIVE_TYPE("FailedFile", "Failed file", "dcd283c3-e61c-476a-8ac6-8e07e863faea", 1);

        FailedFileType();
    };
}
