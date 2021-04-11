#pragma once

#include <cstddef>
#include <functional>

namespace babelwires {

    namespace hash {

        inline void mixInto(std::size_t& seed) {}

        /// Mix the hash of an arbitrary number of objects into a given hash.
        /// Based indirectly on boost::hash::mixInto.
        template <typename T, typename... Rest> inline void mixInto(std::size_t& seed, const T& v, Rest... rest) {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            hash::mixInto(seed, rest...);
        }

        /// Get the hash of an arbitrary number of objects.
        template <typename T, typename... Rest> inline std::size_t mixtureOf(const T& v, Rest... rest) {
            std::hash<T> hasher;
            size_t hash = hasher(v);
            hash::mixInto(hash, rest...);
            return hash;
        }

    } // namespace hash

} // namespace babelwires
