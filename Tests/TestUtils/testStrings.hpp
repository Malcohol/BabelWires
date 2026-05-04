#pragma once

#include <string_view>

namespace testUtils {
    /// Compile-time constant string.
    constexpr std::string_view c_testString = "This is a test string";

    /// This is intended to be a non-compile-time version of the same string. 
    std::string_view getTestStringOutOfLine();
} // namespace testUtils
