/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Hash/hash.hpp>

#include <array>
namespace babelwires {
    class Type;
    class TypeSystem;
    class TypeRef;

    template <unsigned int N> class TypeConstructorArguments {
      public:
        /// The maximum number of arguments a TypeRef can carry.
        static constexpr std::size_t s_maxNumArguments = 10;

        std::array<TypeRef, N> m_typeArguments;

        friend bool operator==(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return a.m_typeArguments == b.m_typeArguments;
        }
        friend bool operator!=(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return a.m_typeArguments != b.m_typeArguments;
        }
        friend bool operator<(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return a.m_typeArguments < b.m_typeArguments;
        }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const {
            std::size_t hash = 0x80235AA2;
            for (const auto& arg : m_typeArguments) {
                hash::mixInto(hash, arg);
            }
            return hash;
        }
    };
} // namespace babelwires

namespace std {
    template <unsigned int N> struct hash<babelwires::TypeConstructorArguments<N>> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments<N>& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std
