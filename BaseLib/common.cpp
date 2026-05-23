/**
 * Some general type definitions, functions and templates.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/common.hpp>

#include <cassert>
#include <cctype>
#include <stdint.h>

bool babelwires::isValidIdentifier(const char* str) {
    assert(str != nullptr);
    return isValidIdentifier(std::string_view(str));
}

bool babelwires::isValidIdentifier(std::string_view str) {
    if (str.empty()) {
        return false;
    }
    if (!isalpha(str.front()) && (str.front() != '_')) {
        return false;
    }
    for (const char c : str) {
        if (!isalpha(c) && !isdigit(c) && (c != '_')) {
            return false;
        }
    }
    return true;
}

std::string babelwires::pathToString(const std::filesystem::path& path) {
    return path.string();
}

