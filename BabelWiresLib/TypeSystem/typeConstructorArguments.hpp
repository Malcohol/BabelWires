/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <vector>

namespace babelwires {
    class Type;
    class TypeSystem;
    class TypeRef;
    class ValueHolder;

    class TypeConstructorArguments {
      public:
        ~TypeConstructorArguments();

        /// The maximum number of arguments a TypeRef can carry.
        static constexpr std::size_t s_maxNumArguments = 10;

        std::vector<TypeRef> m_typeArguments;
        std::vector<ValueHolder> m_valueArguments;

        friend bool operator==(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return (a.m_typeArguments == b.m_typeArguments) && (a.m_valueArguments == b.m_valueArguments);
        }
        friend bool operator!=(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return !(a == b);
        }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std
