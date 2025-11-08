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
        TypeConstructorArguments() = default;
        TypeConstructorArguments(std::vector<TypeRef> typeArguments)
            : m_typeArguments(std::move(typeArguments)) {}
        TypeConstructorArguments(std::vector<TypeRef> typeArguments, std::vector<ValueHolder> valueArguments)
            : m_typeArguments(std::move(typeArguments))
            , m_valueArguments(std::move(valueArguments)) {
#ifndef NDEBUG
            for (const auto& v : valueArguments) {
                assert(v->tryGetAsEditableValue() && "Values stored by TypeConstructors must be editable");
            }
#endif
        }
        ~TypeConstructorArguments();

        const std::vector<TypeRef>& getTypeArguments() const { return m_typeArguments; }
        std::vector<TypeRef>& getTypeArguments() { return m_typeArguments; }
        const std::vector<ValueHolder>& getValueArguments() const { return m_valueArguments; }
        std::vector<ValueHolder>& getValueArguments() { return m_valueArguments; }

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

      private:
        std::vector<TypeRef> m_typeArguments;
        std::vector<ValueHolder> m_valueArguments;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std
