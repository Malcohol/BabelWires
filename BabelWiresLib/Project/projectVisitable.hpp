/**
 * ProjectVisibable defines an interface for classes storing project data, allowing them to expose certain data
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <functional>

namespace babelwires {
    union Identifier;
    class FilePath;

    using FieldVisitor = std::function<void(Identifier&)>;
    using FilePathVisitor = std::function<void(FilePath&)>;

    /// An interface for classes storing project data, allowing them to expose certain data
    /// to important infrastructural visitors.
    struct ProjectVisitable {
        /// Call the visitor on all fields in the modifier.
        virtual void visitFields(FieldVisitor& visitor) = 0;

        /// Call the visitor on all FilePaths in the modifier.
        virtual void visitFilePaths(FilePathVisitor& visitor) = 0;
    };
}
