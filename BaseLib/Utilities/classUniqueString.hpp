/**
 * 
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string_view>

namespace babelwires {

    /// Get a unique string for the class CLASS
    template <typename CLASS> constexpr std::string_view getClassUniqueString() {
#if defined(__clang__) || defined(__GNUC__)
        return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        return __FUNCSIG__;
#else
        return "Unsupported compiler";
#endif
    }

} // namespace babelwires
