/**
 * DataVisitable defines an interface for classes storing certain types of data, allowing it to be exposed
 * to important infrastructural visitors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/DataContext/filePathVisitable.hpp>
#include <Common/Identifiers/identifierVisitable.hpp>

namespace babelwires {
    /// DataVisitable defines an interface for classes storing certain types of data, allowing it to be exposed
    /// to important infrastructural visitors.
    struct DataVisitable : IdentifierVisitable, FilePathVisitable {};
} // namespace babelwires
