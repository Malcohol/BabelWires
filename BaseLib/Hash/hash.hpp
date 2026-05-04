/**
 * Functionality for hashing data.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string_view>

namespace babelwires {

    namespace hash {

        /// Compile-time/runtime stable FNV-1a hash for string data.
        constexpr std::uint64_t stableStringHash(std::string_view text) {
            std::uint64_t hash = 14695981039346656037ull;
            for (char c : text) {
                hash ^= static_cast<unsigned char>(c);
                hash *= 1099511628211ull;
            }
            return hash;
        }

        inline void mixInto(std::size_t& seed) {}

        /// Mix the hash of an arbitrary number of objects into a given hash.
        /// Based indirectly on boost::hash::mixInto.
        template <typename T, typename... Rest> inline void mixInto(std::size_t& seed, const T& v, Rest&&... rest) {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            hash::mixInto(seed, std::forward<Rest>(rest)...);
        }

        /// Get the hash of an arbitrary number of objects.
        template <typename T, typename... Rest> inline std::size_t mixtureOf(const T& v, Rest&&... rest) {
            std::hash<T> hasher;
            size_t hash = hasher(v);
            hash::mixInto(hash, std::forward<Rest>(rest)...);
            return hash;
        }

    } // namespace hash

} // namespace babelwires
