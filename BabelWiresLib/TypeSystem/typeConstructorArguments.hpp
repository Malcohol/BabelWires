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

    class TypeConstructorArguments {
      public:
        ~TypeConstructorArguments();

        std::vector<TypeRef> m_typeArguments;
        std::vector<ValueHolder> m_valueArguments;

        friend bool operator==(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return equals(a, b);
        }
        friend bool operator!=(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return !equals(a, b);
        }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

      private:
        /// The friend operators need to call an out-of-line implementation to avoid an include cycle.
        static bool equals(const TypeConstructorArguments& a, const TypeConstructorArguments& b);
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std
