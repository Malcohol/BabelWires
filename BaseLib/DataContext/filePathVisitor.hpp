/**
 * FilePathVisitable defines an interface for classes storing FilePaths, allowing them to be exposed
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Identifiers/identifierVisitor.hpp>

namespace babelwires {
    class FilePath;
    using FilePathVisitor = std::function<void(FilePath&)>;
}
