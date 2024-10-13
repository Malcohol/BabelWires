/**
 * A type which corresponds to the contents of a file.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/File/fileType.hpp>

babelwires::FileType::FileType(ShortId stepToContents, TypeRef typeOfContents)
    : RecordType({{stepToContents, typeOfContents}}) {}

babelwires::FileType::FileType()
    : RecordType({}) {}

std::string babelwires::FileType::getKind() const {
    return "";
}

babelwires::FailedFileType::FailedFileType()
    : FileType() {}
