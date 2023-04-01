/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <variant>

namespace babelwires {

    /// A ValueHolder is a container which holds a single value.
    /// TODO in-place storage for small values.
    class ValueHolder {
      public:
        ValueHolder() = default;
        ValueHolder(const ValueHolder& other);
        ValueHolder(ValueHolder&& other);

        ValueHolder(const Value& value);
        ValueHolder(Value&& value);
        ValueHolder(std::unique_ptr<Value> ptr);
        ValueHolder(std::shared_ptr<const Value> ptr);

        ValueHolder& operator=(const ValueHolder& other);
        ValueHolder& operator=(ValueHolder&& other);

        ValueHolder& operator=(const Value& value);
        ValueHolder& operator=(Value&& value);
        ValueHolder& operator=(std::unique_ptr<Value> ptr);
        ValueHolder& operator=(std::shared_ptr<const Value> ptr);

        operator bool() const;
        const Value& operator*() const;
        const Value* operator->() const;

        friend bool operator==(const ValueHolder& a, const ValueHolder& b) {
            return (a.m_pointerToValue == b.m_pointerToValue) ||
                   (a.m_pointerToValue && b.m_pointerToValue && (*a.m_pointerToValue == *b.m_pointerToValue));
        }
        friend bool operator==(const ValueHolder& a, const Value* b) {
            return (a.m_pointerToValue.get() == b) ||
                   (a.m_pointerToValue && b && (*a.m_pointerToValue == *b));
        }
        friend bool operator==(const Value* a, const ValueHolder& b) {
            return (a == b.m_pointerToValue.get()) ||
                   (a && b.m_pointerToValue && (*a == *b.m_pointerToValue));
        }
        friend bool operator!=(const ValueHolder& a, const ValueHolder& b) {
            return !(a == b);
        }
        friend bool operator!=(const ValueHolder& a, const Value* b) {
            return !(a == b);
        }
        friend bool operator!=(const Value* a, const ValueHolder& b) {
            return !(a == b);
        }

      private:
        using PointerToValue = std::shared_ptr<const Value>;
        PointerToValue m_pointerToValue;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ValueHolder> {
        inline std::size_t operator()(const babelwires::ValueHolder& valueHolder) const {
            return valueHolder->getHash();
        }
    };
} // namespace std
