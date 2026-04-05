/**
 * Build fingerprint generation for loadable module compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// NO DEPENDENCIES ARE PERMITTED HERE EXCEPT FOR STANDARD LIBRARY HEADERS.
//
// This code is called prior to the point at which it's known that the module is compatible
// with the main codebase.

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <utility>

namespace babelwires {

    inline constexpr std::size_t c_buildFingerprintBufferSize = 1024;

    /// Write the current build fingerprint into a caller-provided buffer.
    ///
    /// The output uses a canonical format with one key per line, sorted lexicographically by key.
    /// The returned value is the number of bytes used in outBuffer, including the null terminator.
    /// If output is truncated, the return value equals bufferSize.
    /// If outBuffer is null or bufferSize is zero, nothing is written and zero is returned.
    inline std::size_t writeBuildFingerprint(char* outBuffer, std::size_t bufferSize) {
        const auto compiler = []() -> std::string {
#if defined(__clang__)
            return std::string("clang ") + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) +
                   "." + std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
            return std::string("gcc ") + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." +
                   std::to_string(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
            return std::string("msvc ") + std::to_string(_MSC_VER);
#else
            return "unknown";
#endif
        }();

        const auto standardLibrary = []() -> std::string {
#if defined(_LIBCPP_VERSION)
            return std::string("libc++ ") + std::to_string(_LIBCPP_VERSION);
#elif defined(__GLIBCXX__)
            return std::string("libstdc++ ") + std::to_string(__GLIBCXX__);
#elif defined(_MSVC_STL_VERSION)
            return std::string("msvc-stl ") + std::to_string(_MSVC_STL_VERSION);
#else
            return "unknown";
#endif
        }();

        const auto buildType = []() -> std::string {
#ifdef NDEBUG
            return "release";
#else
            return "debug";
#endif
        }();

        const auto platform = []() -> std::string {
#if defined(_WIN32)
            return "windows";
#elif defined(__APPLE__)
            return "apple";
#elif defined(__linux__)
            return "linux";
#else
            return "unknown";
#endif
        }();

        const auto endianness = []() -> std::string {
            union {
                unsigned int value;
                unsigned char bytes[sizeof(unsigned int)];
            } data{0x01020304u};
            return (data.bytes[0] == 0x01u) ? "big" : "little";
        }();

        const auto glibcxxAbi = []() -> std::string {
#ifdef _GLIBCXX_USE_CXX11_ABI
            return std::to_string(_GLIBCXX_USE_CXX11_ABI);
#else
            return "0";
#endif
        }();

        std::array<std::pair<std::string, std::string>, 10> fields = {
            std::pair<std::string, std::string>{"build_type", buildType},
            std::pair<std::string, std::string>{"compiler", compiler},
            std::pair<std::string, std::string>{"cxx_standard", std::to_string(__cplusplus)},
            std::pair<std::string, std::string>{"endianness", endianness},
            std::pair<std::string, std::string>{"glibcxx_cxx11_abi", glibcxxAbi},
            std::pair<std::string, std::string>{"platform", platform},
            std::pair<std::string, std::string>{"pointer_size", std::to_string(sizeof(void*))},
            std::pair<std::string, std::string>{"size_of_long", std::to_string(sizeof(long))},
            std::pair<std::string, std::string>{"size_of_wchar_t", std::to_string(sizeof(wchar_t))},
            std::pair<std::string, std::string>{"standard_library", standardLibrary},
        };

        std::sort(fields.begin(), fields.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

        std::string formatted;
        formatted += "schema_version=1\n";
        for (const auto& [key, value] : fields) {
            formatted += key;
            formatted += '=';
            formatted += value;
            formatted += '\n';
        }

        if ((outBuffer == nullptr) || (bufferSize == 0)) {
            return 0;
        }

        const std::size_t contentBytesToWrite = std::min(formatted.size(), bufferSize - 1);
        std::memcpy(outBuffer, formatted.data(), contentBytesToWrite);
        outBuffer[contentBytesToWrite] = 0;
        return contentBytesToWrite + 1;
    }

} // namespace babelwires
