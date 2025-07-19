/**
 * Some utilities for Unicode
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>

namespace babelwires {
    void writeUnicodeSubscript(std::ostream& os, int number);

    void writeUnicodeSuperscript(std::ostream& os, int number);
}
