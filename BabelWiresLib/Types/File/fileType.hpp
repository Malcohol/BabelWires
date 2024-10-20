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
        FileType(TypeRef typeOfContents);

        std::string getKind() const override;
    };
}
