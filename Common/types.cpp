/**
 * Some general type definitions, functions and templates.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/types.hpp>

#include <cassert>
#include <cctype>
#include <stdint.h>

bool babelwires::isValidIdentifier(const char* str) {
    assert(str != nullptr);
    if (*str == 0) {
        return false;
    }
    if (!isalpha(*str) && (*str != '_')) {
        return false;
    }
    for (; *str; ++str) {
        if (!isalpha(*str) && !isdigit(*str) && (*str != '_')) {
            return false;
        }
    }
    return true;
}

std::string babelwires::pathToString(const std::filesystem::path& path) {
    return path.string();
}

