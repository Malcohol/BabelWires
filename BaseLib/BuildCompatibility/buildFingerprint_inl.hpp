/**
 * Build fingerprint generation for loadable module compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

// NO DEPENDENCIES ARE PERMITTED HERE EXCEPT FOR STANDARD LIBRARY HEADERS.
//
// This code is called prior to the point at which it's known that the module is compatible
// with the main codebase.

#include <bit>
#include <cstring>
#include <string>

// This is necessarily an inline function.
inline std::size_t babelwires::writeMyBuildFingerprint(char* outBuffer, std::size_t bufferSize) {
    const auto compiler = []() -> std::string {
#if defined(__clang__)
        return std::string("clang ") + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." +
               std::to_string(__clang_patchlevel__);
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
        if constexpr (std::endian::native == std::endian::big) {
            return "big";
        } else if constexpr (std::endian::native == std::endian::little) {
            return "little";
        } else {
            return "mixed";
        }
    }();

    const auto glibcxxAbi = []() -> std::string {
#ifdef _GLIBCXX_USE_CXX11_ABI
        return std::to_string(_GLIBCXX_USE_CXX11_ABI);
#else
        return "0";
#endif
    }();

    std::string formatted;
    const auto appendField = [&formatted](const char* key, const std::string& value) {
        formatted += key;
        formatted += '=';
        formatted += value;
        formatted += '\n';
    };

    // IMPORTANT: NEW DATA FIELDS MUST BE APPENDED AT THE END. FIELDS MUST NOT BE REORDERED.
    // Also note: It is not required that all fields be present in all variants, but a field should be present in all
    // variants which have the same values of preceding fields (i.e. the fingerprint format is a prefix tree / trie).
    appendField("schema_version", "1");
    appendField("build_type", buildType);
    appendField("compiler", compiler);
    appendField("cxx_standard", std::to_string(__cplusplus));
    appendField("endianness", endianness);
    appendField("glibcxx_cxx11_abi", glibcxxAbi);
    appendField("platform", platform);
    appendField("pointer_size", std::to_string(sizeof(void*)));
    appendField("size_of_long", std::to_string(sizeof(long)));
    appendField("size_of_wchar_t", std::to_string(sizeof(wchar_t)));
    appendField("standard_library", standardLibrary);

    if ((outBuffer == nullptr) || (bufferSize == 0)) {
        return 0;
    }

    const std::size_t contentBytesToWrite = std::min(formatted.size(), bufferSize - 1);
    std::memcpy(outBuffer, formatted.data(), contentBytesToWrite);
    outBuffer[contentBytesToWrite] = 0;
    return contentBytesToWrite + 1;
}