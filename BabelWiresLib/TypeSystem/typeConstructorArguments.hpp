/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <vector>

namespace babelwires {
    class Type;
    class TypeSystem;
    class TypeRef;

    class TypeConstructorArguments {
      public:
        ~TypeConstructorArguments();
        std::vector<TypeRef> m_typeArguments;
        friend bool operator==(const TypeConstructorArguments& a, const TypeConstructorArguments& b) { return a.m_typeArguments == b.m_typeArguments; }
        friend bool operator!=(const TypeConstructorArguments& a, const TypeConstructorArguments& b) { return a.m_typeArguments != b.m_typeArguments; }
        friend bool operator<(const TypeConstructorArguments& a, const TypeConstructorArguments& b) { return a.m_typeArguments < b.m_typeArguments; }
        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;
    };
}

namespace std {
    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const { return arguments.getHash(); }
    };
} // namespace std
