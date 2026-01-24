/**
 * DataVisitable defines an interface for classes storing certain types of data, allowing it to be exposed
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/DataContext/filePathVisitor.hpp>
#include <BaseLib/Identifiers/identifierVisitor.hpp>

namespace babelwires {
    /// DataVisitable defines an interface for classes storing certain types of data, allowing it to be exposed
    /// to important infrastructural visitors.
    struct DataVisitable {
        /// Call the visitor on all Identifiers in the object.
        virtual void visitIdentifiers(IdentifierVisitor& visitor) = 0;

        /// Call the visitor on all FilePaths in the object.
        virtual void visitFilePaths(FilePathVisitor& visitor) = 0;
    };
} // namespace babelwires
