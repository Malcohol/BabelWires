/**
 * FilePathVisitable defines an interface for classes storing FilePaths, allowing them to be exposed
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifierVisitable.hpp>

namespace babelwires {
   
    class FilePath;
    using FilePathVisitor = std::function<void(FilePath&)>;

    /// FilePathVisitable defines an interface for classes storing FilePaths, allowing them to be exposed
    /// to important infrastructural visitors.
    struct FilePathVisitable {
        /// Call the visitor on all FilePaths in the object.
        virtual void visitFilePaths(FilePathVisitor& visitor) = 0;
    };
}
