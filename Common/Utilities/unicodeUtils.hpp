/**
 * Some utilities for Unicode
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <ostream>

namespace babelwires {
    /// Write the number using unicode subscript characters.
    void writeUnicodeSubscript(std::ostream& os, int number);

    /// Write the number using unicode superscript characters
    void writeUnicodeSuperscript(std::ostream& os, int number);
}
